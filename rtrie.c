#include <stdlib.h>
#include <errno.h>
#include "collection.h"
#include "rtrie.h"
#include "bstr.h"

/*
 * allocate a new node
 */
cp_rtrie_node *cp_rtrie_node_new(void *leaf, cp_mempool *pool) 
{ 
	cp_rtrie_node *node;
	if (pool) 
		node = (cp_rtrie_node *) cp_mempool_calloc(pool);
	else
		node = (cp_rtrie_node *) calloc(1, sizeof(cp_rtrie_node)); 

	if (node) 
		node->leaf = leaf; 

	return node; 
} 

/*
 * recursively delete the subtree at node
 */
void *cp_rtrie_node_delete(cp_rtrie *grp, cp_rtrie_node *node) 
{ 
	void *leaf = NULL; 

	if (node) 
	{ 
		if (node->node_zero)
			cp_rtrie_node_delete(grp, node->node_zero);
		if (node->zero)
			cp_bstr_destroy(node->zero);
		if (node->node_one)
			cp_rtrie_node_delete(grp, node->node_one);
		if (node->one)
			cp_bstr_destroy(node->one);

		leaf = node->leaf; 
		if (grp->mempool)
			cp_mempool_free(grp->mempool, node);
		else
			free(node); 
	} 

	if (leaf && grp->delete_leaf && grp->mode & COLLECTION_MODE_DEEP)
		(*grp->delete_leaf)(leaf);

	return leaf; 
} 

/*
 * delete just one node (no recursion) 
 */
void *cp_rtrie_node_drop(cp_rtrie *grp, cp_rtrie_node *node) 
{ 
	void *leaf = NULL; 

	if (node) 
	{ 
		if (node->zero)
			cp_bstr_destroy(node->zero);
		if (node->one)
			cp_bstr_destroy(node->one);

		leaf = node->leaf; 
		if (grp->mempool)
			cp_mempool_free(grp->mempool, node);
		else
			free(node); 
	} 

	if (leaf && grp->delete_leaf && grp->mode & COLLECTION_MODE_DEEP)
		(*grp->delete_leaf)(leaf);

	return leaf; 
} 

void cp_rtrie_node_unmap(cp_rtrie *grp, cp_rtrie_node **node) 
{ 
	cp_rtrie_node_delete(grp, *node); 
	*node = NULL; 
} 

int cp_rtrie_lock_internal(cp_rtrie *grp, int type)
{
	int rc = 0;
	switch(type)
	{
		case COLLECTION_LOCK_READ:
			rc = cp_lock_rdlock(grp->lock);
			break;

		case COLLECTION_LOCK_WRITE:
			rc = cp_lock_wrlock(grp->lock);
			break;

		case COLLECTION_LOCK_NONE:
		default:
			break; 
	}

	return rc;
}

int cp_rtrie_unlock_internal(cp_rtrie *grp)
{
	return cp_lock_unlock(grp->lock);
}

int cp_rtrie_txlock(cp_rtrie *grp, int type)
{
	if (grp->mode & COLLECTION_MODE_NOSYNC) return 0;
	if (grp->mode & COLLECTION_MODE_IN_TRANSACTION && 
		grp->txtype == COLLECTION_LOCK_WRITE)
	{
		cp_thread self = cp_thread_self();
		if (cp_thread_equal(self, grp->txowner)) return 0;
	}
	return cp_rtrie_lock_internal(grp, type);
}

int cp_rtrie_txunlock(cp_rtrie *grp)
{
	if (grp->mode & COLLECTION_MODE_NOSYNC) return 0;
	if (grp->mode & COLLECTION_MODE_IN_TRANSACTION && 
		grp->txtype == COLLECTION_LOCK_WRITE)
	{
		cp_thread self = cp_thread_self();
		if (cp_thread_equal(self, grp->txowner)) return 0;
	}
	return cp_rtrie_unlock_internal(grp);
}

/* lock and set the transaction indicators */
int cp_rtrie_lock(cp_rtrie *grp, int type)
{
	int rc;
	if ((grp->mode & COLLECTION_MODE_NOSYNC)) return EINVAL;
	if ((rc = cp_rtrie_lock_internal(grp, type))) return rc;
	grp->txtype = type;
	grp->txowner = cp_thread_self();
	grp->mode |= COLLECTION_MODE_IN_TRANSACTION;
	return 0;
}

/* unset the transaction indicators and unlock */
int cp_rtrie_unlock(cp_rtrie *grp)
{
	cp_thread self = cp_thread_self();
	if (grp->txowner == self)
	{
		grp->txtype = 0;
		grp->txowner = 0;
		grp->mode ^= COLLECTION_MODE_IN_TRANSACTION;
	}
	else if (grp->txtype == COLLECTION_LOCK_WRITE)
		return -1;
	return cp_rtrie_unlock_internal(grp);
}

/* get the current collection mode */
int cp_rtrie_get_mode(cp_rtrie *grp)
{
    return grp->mode;
}

/* set mode bits on the rtrie mode indicator */
int cp_rtrie_set_mode(cp_rtrie *grp, int mode)
{
	int nosync;

	/* can't set NOSYNC in the middle of a transaction */
	if ((grp->mode & COLLECTION_MODE_IN_TRANSACTION) && 
		(mode & COLLECTION_MODE_NOSYNC)) return EINVAL;
	
	nosync = grp->mode & COLLECTION_MODE_NOSYNC;
	if (!nosync)
		if (cp_rtrie_txlock(grp, COLLECTION_LOCK_WRITE))
			return -1;

	grp->mode |= mode;

	if (!nosync)
		cp_rtrie_txunlock(grp);

	return 0;
}

/* unset mode bits on the grp mode indicator. if unsetting 
 * COLLECTION_MODE_NOSYNC and the grp was not previously synchronized, the 
 * internal synchronization structure is initalized.
 */
int cp_rtrie_unset_mode(cp_rtrie *grp, int mode)
{
	int nosync = grp->mode & COLLECTION_MODE_NOSYNC;

	if (!nosync)
		if (cp_rtrie_txlock(grp, COLLECTION_LOCK_WRITE))
			return -1;
	
	/* handle the special case of unsetting COLLECTION_MODE_NOSYNC */
	if ((mode & COLLECTION_MODE_NOSYNC) && grp->lock == NULL)
	{
		/* grp can't be locked in this case, no need to unlock on failure */
		if ((grp->lock = malloc(sizeof(cp_lock))) == NULL)
			return -1; 
		if (cp_lock_init(grp->lock, NULL))
			return -1;
	}
	
	/* unset specified bits */
    grp->mode &= grp->mode ^ mode;
	if (!nosync)
		cp_rtrie_txunlock(grp);

	return 0;
}


cp_rtrie *cp_rtrie_create_rtrie(int mode,
		                     cp_copy_fn copy_leaf,
							 cp_destructor_fn delete_leaf) 
{ 
	cp_rtrie *grp = calloc(1, sizeof(cp_rtrie)); 

	if (grp == NULL) return NULL; 
	grp->root = cp_rtrie_node_new(NULL, NULL); //~~ what if mempool set later?
	if (grp->root == NULL) 
	{ 
		free(grp); 
		return NULL; 
	} 

	if (!(mode & COLLECTION_MODE_NOSYNC))
	{
		if ((grp->lock = malloc(sizeof(cp_lock))) == NULL)
		{
			cp_rtrie_node_delete(grp, grp->root);
			free(grp);
			return NULL;
		}

		if (cp_lock_init(grp->lock, NULL)) 
		{
			free(grp->lock);
			cp_rtrie_node_delete(grp, grp->root);
			free(grp); 
			return NULL; 
		} 
	}

	grp->mode = mode;
	grp->copy_leaf = copy_leaf;
	grp->delete_leaf = delete_leaf;

	return grp; 
} 

cp_rtrie *cp_rtrie_create(int mode)
{
	return cp_rtrie_create_rtrie(mode, NULL, NULL);
}

/*
 * recursively deletes rtrie structure
 */
int cp_rtrie_destroy(cp_rtrie *grp) 
{ 
	int rc = 0; 

	/* no locking is done here. It is the application's responsibility to
	 * ensure that the rtrie isn't being destroyed while it's still in use
	 * by other threads.
	 */
	if (grp)
	{ 
		cp_rtrie_node_delete(grp, grp->root); 
		if (grp->lock) 
		{
			rc = cp_lock_destroy(grp->lock); 
			free(grp->lock);
		}
		free(grp); 
	} 
	else rc = -1; 

	return rc; 
} 

void *RNODE_STORE(cp_rtrie_node *node, cp_bstr *key, cp_rtrie_node *sub)
{
	if ((key->bits[0] & 0x80) == 0)
	{
		node->zero = key;
		node->node_zero = sub;
	}
	else
	{
		node->one = key;
		node->node_one = sub;
	}

	return sub; // mtab_put(node->others, *k, sub, k);
}

void *RNODE_STORE_CPY(cp_rtrie_node *node, cp_bstr *key, cp_rtrie_node *sub)
{
	cp_bstr *k = cp_bstr_dup(key);
	if (k == NULL) return NULL;

	if ((k->bits[0] & 0x80) == 0)
	{
		node->zero = k;
		node->node_zero = sub;
	}
	else
	{
		node->one = k;
		node->node_one = sub;
	}

	return sub; // mtab_put(node->others, *k, sub, k);
}

cp_rtrie_node **RNODE_MATCH(cp_rtrie_node *node, cp_bstr *key, int *pos)
{
	*pos = -1;
	if (node->zero)
	{
		cp_bstr_cmp(node->zero, key, pos);
//		if (*pos != -1) 
		if (*pos > 0)
			return &node->node_zero;
	}

	if (node->one)
	{
		cp_bstr_cmp(node->one, key, pos);
//		if (*pos != -1) 
		if (*pos > 0)
			return &node->node_one;
	}

	return NULL;
}
	
/*
 * since cp_rtrie compresses single child nodes, the following cases are handled
 * here. 'abc' etc denote existing path, X marks the spot:
 *
 * (1) simple case: abc-X         - first mapping for abc
 *
 * (2) mid-branch:  ab-X-c        - abc already mapped, but ab isn't 
 *
 * (3) new branch:  ab-X-cd       - the key abcd is already mapped
 *                      \          
 *                       ef         the key abef is to be added
 *
 * (4) extending:   abc-de-X      - abc mapped, abcde isn't
 * 
 * (5) replace:     abc-X         - abc already mapped, just replace leaf  
 */
int cp_rtrie_node_store(cp_rtrie *grp, 
                       cp_rtrie_node *node, 
					   cp_bstr *key, 
					   void *leaf) 
{ 
//	mtab_node *map_node;
//	char *next;
	int pos;
	cp_rtrie_node *sub, *old;
	cp_bstr *curr;

//	map_node = RNODE_MATCH(node, key); 

	cp_rtrie_node **match = RNODE_MATCH(node, key, &pos);
	if (match == NULL) /* case (1) - not mapped - just add it */
	{ 
		sub = cp_rtrie_node_new(leaf, grp->mempool); 
		if (RNODE_STORE_CPY(node, key, sub) == NULL) return -1; 
	} 
	else 
	{
		curr = (*match == node->node_zero) ? node->zero : node->one;
		if (pos == key->length)
		{
			if (pos < curr->length) /* case (2) - key ab, curr abc */
			{
				cp_bstr *right = cp_bstr_dup(curr);
				if (right == NULL) return -1;
				//~~ cp_bstr_dup_shift would save potential memory wastage here
				cp_bstr_shift_left(right, pos); /* key ab, right c */
				sub = cp_rtrie_node_new(leaf, grp->mempool);
				if (sub == NULL) return -1;
				if (RNODE_STORE(sub, right, *match) == NULL) return -1;
				sub->leaf = leaf;
				*match = sub; /* set old pointer to new node */
				curr->length = pos; /* adjust old bit sequence length */
			}
			else if (pos == curr->length) /* case (5) - replace: key abc, curr abc */
			{
				if ((*match)->leaf && grp->delete_leaf && 
					((grp->mode & COLLECTION_MODE_DEEP) != 0))
					(*grp->delete_leaf)((*match)->leaf);
				(*match)->leaf = leaf;
			}
		}
		else /* pos < key->length */
		{
			if (pos == curr->length) /* case (4) - extend */
			{
				int res;
				cp_bstr *right = cp_bstr_dup(key);
				if (right == NULL) return -1;
				cp_bstr_shift_left(right, pos);
				res = cp_rtrie_node_store(grp, *match, right, leaf);
				cp_bstr_destroy(right);
				return res;
			}
			else /* pos can't be greater than curr length, so it must be shorter - case (3) */
			{
				cp_bstr *snew, *sold;

				snew = cp_bstr_dup(key);
				if (snew == NULL) return -1;
				sold = cp_bstr_dup(curr);
				if (sold == NULL) return -1;
				sub = cp_rtrie_node_new(leaf, grp->mempool);
				if (sub == NULL) return -1;
				old = cp_rtrie_node_new(NULL, grp->mempool);
				if (old == NULL) return -1;

				cp_bstr_shift_left(snew, pos);
				cp_bstr_shift_left(sold, pos);
				RNODE_STORE(old, sold, *match);
				RNODE_STORE(old, snew, sub);
				curr->length = pos;
				*match = old;
			}
		}
	}

	return 0;
} 

/*
 * wrapper for the recursive insertion - implements collection mode setting
 */
int cp_rtrie_add(cp_rtrie *grp, cp_bstr *key, void *leaf) 
{ 
	int rc = 0; 
	if ((rc = cp_rtrie_txlock(grp, COLLECTION_LOCK_WRITE))) return rc; 

	if ((grp->mode & COLLECTION_MODE_COPY) && grp->copy_leaf && (leaf != NULL))
	{
		leaf = (*grp->copy_leaf)(leaf);
		if (leaf == NULL) goto DONE;
	}

	if (key == NULL) /* map NULL key to root node */
	{
		if (grp->root->leaf && 
				grp->delete_leaf && grp->mode & COLLECTION_MODE_DEEP)
			(*grp->delete_leaf)(grp->root->leaf);
		grp->root->leaf = leaf; 
	}
	else 
	{
		if ((rc = cp_rtrie_node_store(grp, grp->root, key, leaf)))
			goto DONE;
	} 

	grp->path_count++; 

DONE:
	cp_rtrie_txunlock(grp);
	return rc; 
} 

#if 0
/* helper functions for cp_rtrie_remove */

static char *mergestr(char *s1, char *s2)
{
	char *s;
	int len = strlen(s1) + strlen(s2);

	s = malloc((len + 1) * sizeof(char));
	if (s == NULL) return NULL;
	
#ifdef CP_HAS_STRLCPY
	strlcpy(s, s1, len + 1);
#else
	strcpy(s, s1);
#endif /* CP_HAS_STRLCPY */
#ifdef CP_HAS_STRLCAT
	strlcat(s, s2, len + 1);
#else
	strcat(s, s2);
#endif /* CP_HAS_STRLCAT */

	return s;
}

static mtab_node *get_single_entry(mtab *t)
{
	int i;

	for (i = 0; i < t->size; i++)
		if (t->table[i]) return t->table[i];

	return NULL;
}

#endif

/*
 * removing mappings, the following cases are possible:
 * 
 * (1) simple case:       abc-X       removing mapping abc
 *
 * (2) branch:            abc-de-X    removing mapping abcde -
 *                           \        mapping abcfg remains, but
 *                            fg      junction node abc no longer needed
 *
 * (3) mid-branch:        abc-X-de    removing mapping abc - mapping abcde
 *                                    remains
 */
int cp_rtrie_remove(cp_rtrie *grp, cp_bstr *key, void **leaf)
{
	int rc = 0;
	cp_rtrie_node *link = grp->root; 
	cp_rtrie_node *prev = NULL;
	int pos;
	cp_rtrie_node **match;
	cp_bstr *k, **kmatch;

	if (link == NULL) return 0; /* tree is empty */

	if ((rc = cp_rtrie_txlock(grp, COLLECTION_LOCK_READ))) return rc;
 
	if (key == NULL) /* as a special case, NULL keys are stored on the root */
	{
		if (link->leaf)
		{
			if (leaf != NULL) *leaf = link->leaf;
			grp->path_count--;
   			if (grp->delete_leaf && grp->mode & COLLECTION_MODE_DEEP)
           		(*grp->delete_leaf)(link->leaf);
			link->leaf = NULL;
		}
		goto DONE;
	}

	k = cp_bstr_dup(key);
	while (1)
	{
		pos = -1;
		match = RNODE_MATCH(link, k, &pos);
		if (match == NULL) break;
		kmatch = ((*match) == link->node_zero) ? &link->zero : &link->one;

		if (pos == k->length)
		{
			if (leaf != NULL) *leaf = (*match)->leaf;

			if ((*match)->leaf != NULL)
			{
				grp->path_count--;
				rc = 1;
       			if (grp->delete_leaf && grp->mode & COLLECTION_MODE_DEEP)
               		(*grp->delete_leaf)((*match)->leaf);
				(*match)->leaf = NULL;
			}

			if ((*match)->zero != NULL && (*match)->one == NULL)
			{
				cp_bstr_cat(*kmatch, (*match)->zero);
				link = *match;
				*match = (*match)->node_zero;
				cp_rtrie_node_drop(grp, link);
			}
			else if ((*match)->zero == NULL && (*match)->one != NULL)
			{
				cp_bstr_cat(*kmatch, (*match)->one);
				link = *match;
				*match = (*match)->node_one;
				cp_rtrie_node_drop(grp, link);
			}
			else if ((*match)->zero == NULL && (*match)->one == NULL)
			{
				cp_rtrie_node_drop(grp, *match);
				*match = NULL;
				cp_bstr_destroy(*kmatch);
				*kmatch = NULL;

				/* handle the dangle - case (2) */
				if (prev != NULL && link->leaf == NULL)
				{
					if (prev->node_zero == link)
					{
						match = &prev->node_zero;
						kmatch = &prev->zero;
					}
					else
					{
						match = &prev->node_one;
						kmatch = &prev->one;
					}
					if (link->zero == NULL)
					{
						*match = link->node_one;
						cp_bstr_cat(*kmatch, link->one);
					}
					else
					{
						*match = link->node_zero;
						cp_bstr_cat(*kmatch, link->zero);
					}
					cp_rtrie_node_drop(grp, link);
				}
			}

			goto DONE;
		}
		else if (pos == (*kmatch)->length) // pos > 0)
			cp_bstr_shift_left(k, pos);
		else /* not found */
			break; 

		prev = link;
		link = *match;
	}

DONE: 
	cp_bstr_destroy(k);
	cp_rtrie_txunlock(grp);
	return rc;
	
}

void *cp_rtrie_exact_match(cp_rtrie *grp, cp_bstr *key)
{
	void *res = NULL;
	cp_bstr *k, *kmatch;
	cp_rtrie_node *curr, **match;
	int pos;

	k = NULL;
	if (cp_rtrie_txlock(grp, COLLECTION_LOCK_READ)) return NULL;

	if (key == NULL) 
	{
		res = grp->root->leaf;
		goto DONE;
	}

	k = cp_bstr_dup(key);
	curr = grp->root;
	while (1)
	{
		pos = -1;
		match = RNODE_MATCH(curr, k, &pos);
		if (match == NULL) break;
		kmatch = ((*match) == curr->node_zero) ? curr->zero : curr->one;
		if (pos == k->length)
		{
			if (pos == kmatch->length)
				res = (*match)->leaf;
			break;
		}
		if (pos == kmatch->length)
			cp_bstr_shift_left(k, pos);
		else
			break;

		curr = *match;
	}

DONE:
	if (k != NULL)
		cp_bstr_destroy(k);
	cp_rtrie_txunlock(grp);
	return res;
}

cp_vector *cp_rtrie_fetch_matches(cp_rtrie *grp, cp_bstr *key)
{
	int rc;
	cp_vector *res = NULL;
	cp_rtrie_node *curr = grp->root;
	cp_bstr *k = NULL;
	int pos;
	cp_rtrie_node **match;
	cp_bstr *kmatch;

	if ((rc = cp_rtrie_txlock(grp, COLLECTION_LOCK_READ))) return NULL; 
	
	if (key == NULL) 
	{
		res = cp_vector_create(1);
		if (res == NULL)
			goto DONE;

		errno = 0;
		if (cp_vector_add_element(res, curr->leaf) == NULL)
		{
			if (errno != 0)
			{
				cp_vector_destroy(res);
				res = NULL;
			}
		}
		goto DONE;
	}

	k = cp_bstr_dup(key);
	curr = grp->root;
	while (1)
	{
		pos = -1;
		match = RNODE_MATCH(curr, k, &pos);
		if (match == NULL) break;
		kmatch = ((*match) == curr->node_zero) ? curr->zero : curr->one;

		if (res == NULL)
		{
			res = cp_vector_create(1);
			if (res == NULL) //~~ set errno
				goto DONE;
		}
		errno = 0;
		if (cp_vector_add_element(res, curr->leaf) == NULL && errno != 0)
		{
			cp_vector_destroy(res);
			res = NULL;
			goto DONE;
		}

		if (pos == k->length)
		{
			if (pos == kmatch->length)
			{
				errno = 0;
				if (cp_vector_add_element(res, (*match)->leaf) == NULL && errno != 0)
				{
					cp_vector_destroy(res);
					res = NULL;
				}
			}
			break;
		}
		if (pos == kmatch->length)
			cp_bstr_shift_left(k, pos);
		else
			break;

		curr = *match;
	}

DONE:
	if (k != NULL)
		cp_bstr_destroy(k);
	cp_rtrie_txunlock(grp);
	return res; 
}


static void extract_subrtrie(cp_rtrie_node *link, cp_vector *v)
{
	if (link->leaf)
		cp_vector_add_element(v, link->leaf);

	if (link->node_zero)
		extract_subrtrie(link->node_zero, v);

	if (link->node_one)
		extract_subrtrie(link->node_one, v);
}

/* return a vector containing all enrtries in subtree under path given by key */
cp_vector *cp_rtrie_submatch(cp_rtrie *grp, cp_bstr *key)
{
	cp_vector *res = NULL;
	cp_bstr *k, *kmatch;
	cp_rtrie_node *curr, **match;
	int pos;

	k = NULL;

	if (cp_rtrie_txlock(grp, COLLECTION_LOCK_READ)) return NULL;

	if (key == NULL) 
	{
		res = cp_vector_create(1);
		if (res != NULL)
			extract_subrtrie(grp->root, res);
		goto DONE;
	}

	k = cp_bstr_dup(key);
	curr = grp->root;
	while (1)
	{
		pos = -1;
		match = RNODE_MATCH(curr, k, &pos);
		if (match == NULL) break;
		kmatch = ((*match) == curr->node_zero) ? curr->zero : curr->one;
		if (pos == k->length)
		{
			if (pos == kmatch->length)
			{
				res = cp_vector_create(1);
				if (res != NULL)
					extract_subrtrie(*match, res);
			}
			break;
		}
		if (pos == kmatch->length)
			cp_bstr_shift_left(k, pos);
		else
			break;

		curr = *match;
	}

DONE:
	if (k != NULL)
		cp_bstr_destroy(k);
	cp_rtrie_txunlock(grp);
	return res;
} 

/* return longest prefix match for given key */
int cp_rtrie_prefix_match(cp_rtrie *grp, cp_bstr *key, void **leaf)
{ 
	int count = 0;
	cp_bstr *k, *kmatch;
	cp_rtrie_node *curr, **match;
	int pos;
	void *last = grp->root->leaf; 

	k = NULL;
	if (cp_rtrie_txlock(grp, COLLECTION_LOCK_READ)) return -1;

	if (key == NULL) 
	{
		if (leaf != NULL)
			*leaf = grp->root->leaf;
		goto DONE;
	}

	k = cp_bstr_dup(key);
	curr = grp->root;
	while (1)
	{
		pos = -1;
		match = RNODE_MATCH(curr, k, &pos);
		if (match == NULL) break;
		kmatch = ((*match) == curr->node_zero) ? curr->zero : curr->one;
		if (pos == k->length)
		{
			if (pos == kmatch->length)
			{
				if ((*match)->leaf != NULL)
				{
					last = (*match)->leaf;
					count++;
				}
			}
			break;
		}
		if (pos == kmatch->length)
		{
			cp_bstr_shift_left(k, pos);
			if ((*match)->leaf != NULL)
			{
				last = (*match)->leaf;
				count++;
			}
		}
		else
			break;

		curr = *match;
	}

DONE:
	if (k != NULL)
		cp_bstr_destroy(k);
	if (leaf != NULL)
		*leaf = last;
	cp_rtrie_txunlock(grp);
	return count;
}

int cp_rtrie_count(cp_rtrie *grp)
{
	return grp->path_count;
}

void cp_rtrie_set_root(cp_rtrie *grp, void *leaf) 
{ 
    grp->root->leaf = leaf; 
} 

/* dump rtrie to stdout - for debugging */
void cp_rtrie_dump_node(cp_rtrie_node *node, int level, cp_bstr *prefix)
{
	char *bstr;
	cp_bstr *p;
	int tab;

	if (node->node_zero != NULL)
	{
		p = cp_bstr_dup(prefix);
		cp_bstr_cat(p, node->zero);
		cp_rtrie_dump_node(node->node_zero, level + 1, p);
		cp_bstr_destroy(p);
	}

	bstr = cp_bstr_to_string(prefix);
	for (tab = 0; tab < level; tab++)
		printf("\t");
	printf("%s => [%s]\n", bstr, node->leaf ? (char *) node->leaf : "NULL");
	free(bstr);

	if (node->node_one != NULL)
	{
		p = cp_bstr_dup(prefix);
		cp_bstr_cat(p, node->one);
		cp_rtrie_dump_node(node->node_one, level + 1, p);
		cp_bstr_destroy(p);
	}
}
		
void cp_rtrie_dump(cp_rtrie *grp)
{
	cp_bstr prefix;

	prefix.bits = "";
	prefix.length = 0;
	
	cp_rtrie_dump_node(grp->root, 0, &prefix);
}

/* set rtrie to use given mempool or allocate a new one if pool is NULL */
int cp_rtrie_use_mempool(cp_rtrie *rtrie, cp_mempool *pool)
{
	int rc = 0;
	
	if ((rc = cp_rtrie_txlock(rtrie, COLLECTION_LOCK_WRITE))) return rc;
	
	if (pool)
	{
		if (pool->item_size < sizeof(cp_rtrie_node))
		{
			rc = EINVAL;
			goto DONE;
		}
		if (rtrie->mempool) 
		{
			if (rtrie->path_count) 
			{
				rc = ENOTEMPTY;
				goto DONE;
			}
			cp_mempool_destroy(rtrie->mempool);
		}
		cp_mempool_inc_refcount(pool);
		rtrie->mempool = pool;
	}
	else
	{
		rtrie->mempool = 
			cp_mempool_create_by_option(COLLECTION_MODE_NOSYNC, 
										sizeof(cp_rtrie_node), 0);
		if (rtrie->mempool == NULL) 
		{
			rc = ENOMEM;
			goto DONE;
		}
	}

DONE:
	cp_rtrie_txunlock(rtrie);
	return rc;
}


/* set rtrie to use a shared memory pool */
int cp_rtrie_share_mempool(cp_rtrie *rtrie, cp_shared_mempool *pool)
{
	int rc;

	if ((rc = cp_rtrie_txlock(rtrie, COLLECTION_LOCK_WRITE))) return rc;

	if (rtrie->mempool)
	{
		if (rtrie->path_count)
		{
			rc = ENOTEMPTY;
			goto DONE;
		}

		cp_mempool_destroy(rtrie->mempool);
	}

	rtrie->mempool = cp_shared_mempool_register(pool, sizeof(cp_rtrie_node));
	if (rtrie->mempool == NULL) 
	{
		rc = ENOMEM;
		goto DONE;
	}
	
DONE:
	cp_rtrie_txunlock(rtrie);
	return rc;
}

