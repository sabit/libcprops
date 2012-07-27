#include <stdlib.h>
#include "collection.h"
#include "wtrie.h"
#include "wtab.h"

/*
 * allocate a new node
 */
cp_wtrie_node *cp_wtrie_node_new(void *leaf, cp_mempool *pool) 
{ 
	cp_wtrie_node *node;
	if (pool) 
		node = (cp_wtrie_node *) cp_mempool_calloc(pool);
	else
		node = (cp_wtrie_node *) calloc(1, sizeof(cp_wtrie_node)); 
	if (node) 
	{ 
		node->others = wtab_new(0); 
		node->leaf = leaf; 
	} 

	return node; 
} 

/*
 * recursively delete the subtree at node
 */
void *cp_wtrie_node_delete(cp_wtrie *grp, cp_wtrie_node *node) 
{ 
	void *leaf = NULL; 

	if (node) 
	{ 
		wtab_delete_custom(node->others, grp, (wtab_dtr)cp_wtrie_delete_mapping);
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
 * uses the wtab_dtr 'owner' parameter (grp here) to recursively delete 
 * sub-nodes
 */
void cp_wtrie_delete_mapping(cp_wtrie *grp, wtab_node *map_node)
{
	if (map_node)
	{
		if (map_node->attr) free(map_node->attr);
		if (map_node->value) cp_wtrie_node_delete(grp, map_node->value);
	}
}

void cp_wtrie_node_unmap(cp_wtrie *grp, cp_wtrie_node **node) 
{ 
	cp_wtrie_node_delete(grp, *node); 
	*node = NULL; 
} 

int cp_wtrie_lock_internal(cp_wtrie *grp, int type)
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

int cp_wtrie_unlock_internal(cp_wtrie *grp)
{
	return cp_lock_unlock(grp->lock);
}

int cp_wtrie_txlock(cp_wtrie *grp, int type)
{
	if (grp->mode & COLLECTION_MODE_NOSYNC) return 0;
	if (grp->mode & COLLECTION_MODE_IN_TRANSACTION && 
		grp->txtype == COLLECTION_LOCK_WRITE)
	{
		cp_thread self = cp_thread_self();
		if (cp_thread_equal(self, grp->txowner)) return 0;
	}
	return cp_wtrie_lock_internal(grp, type);
}

int cp_wtrie_txunlock(cp_wtrie *grp)
{
	if (grp->mode & COLLECTION_MODE_NOSYNC) return 0;
	if (grp->mode & COLLECTION_MODE_IN_TRANSACTION && 
		grp->txtype == COLLECTION_LOCK_WRITE)
	{
		cp_thread self = cp_thread_self();
		if (cp_thread_equal(self, grp->txowner)) return 0;
	}
	return cp_wtrie_unlock_internal(grp);
}

/* lock and set the transaction indicators */
int cp_wtrie_lock(cp_wtrie *grp, int type)
{
	int rc;
	if ((grp->mode & COLLECTION_MODE_NOSYNC)) return EINVAL;
	if ((rc = cp_wtrie_lock_internal(grp, type))) return rc;
	grp->txtype = type;
	grp->txowner = cp_thread_self();
	grp->mode |= COLLECTION_MODE_IN_TRANSACTION;
	return 0;
}

/* unset the transaction indicators and unlock */
int cp_wtrie_unlock(cp_wtrie *grp)
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
	return cp_wtrie_unlock_internal(grp);
}

/* get the current collection mode */
int cp_wtrie_get_mode(cp_wtrie *grp)
{
    return grp->mode;
}

/* set mode bits on the wtrie mode indicator */
int cp_wtrie_set_mode(cp_wtrie *grp, int mode)
{
	int nosync;

	/* can't set NOSYNC in the middle of a transaction */
	if ((grp->mode & COLLECTION_MODE_IN_TRANSACTION) && 
		(mode & COLLECTION_MODE_NOSYNC)) return EINVAL;
	
	nosync = grp->mode & COLLECTION_MODE_NOSYNC;
	if (!nosync)
		if (cp_wtrie_txlock(grp, COLLECTION_LOCK_WRITE))
			return -1;

	grp->mode |= mode;

	if (!nosync)
		cp_wtrie_txunlock(grp);

	return 0;
}

/* unset mode bits on the grp mode indicator. if unsetting 
 * COLLECTION_MODE_NOSYNC and the grp was not previously synchronized, the 
 * internal synchronization structure is initalized.
 */
int cp_wtrie_unset_mode(cp_wtrie *grp, int mode)
{
	int nosync = grp->mode & COLLECTION_MODE_NOSYNC;

	if (!nosync)
		if (cp_wtrie_txlock(grp, COLLECTION_LOCK_WRITE))
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
		cp_wtrie_txunlock(grp);

	return 0;
}


cp_wtrie *cp_wtrie_create_wtrie(int mode,
		                     cp_copy_fn copy_leaf,
							 cp_destructor_fn delete_leaf) 
{ 
	cp_wtrie *grp = calloc(1, sizeof(cp_wtrie)); 

	if (grp == NULL) return NULL; 
	grp->root = cp_wtrie_node_new(NULL, NULL); //~~ what if mempool set later?
	if (grp->root == NULL) 
	{ 
		free(grp); 
		return NULL; 
	} 

	if (!(mode & COLLECTION_MODE_NOSYNC))
	{
		if ((grp->lock = malloc(sizeof(cp_lock))) == NULL)
		{
			cp_wtrie_node_delete(grp, grp->root);
			free(grp);
			return NULL;
		}

		if (cp_lock_init(grp->lock, NULL)) 
		{
			free(grp->lock);
			cp_wtrie_node_delete(grp, grp->root);
			free(grp); 
			return NULL; 
		} 
	}

	grp->mode = mode;
	grp->copy_leaf = copy_leaf;
	grp->delete_leaf = delete_leaf;

	return grp; 
} 

cp_wtrie *cp_wtrie_create(int mode)
{
	return cp_wtrie_create_wtrie(mode, NULL, NULL);
}

/*
 * recursively deletes wtrie structure
 */
int cp_wtrie_destroy(cp_wtrie *grp) 
{ 
	int rc = 0; 

	/* no locking is done here. It is the application's responsibility to
	 * ensure that the wtrie isn't being destroyed while it's still in use
	 * by other threads.
	 */
	if (grp)
	{ 
		cp_wtrie_node_delete(grp, grp->root); 
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

void *WNODE_STORE(cp_wtrie_node *node, wchar_t *key, cp_wtrie_node *sub)
{
	wchar_t *k = wcsdup(key); 
	if (k == NULL) return NULL;
	return wtab_put(node->others, *k, sub, k);
}

/*
 * since cp_wtrie compresses single child nodes, the following cases are handled
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
int cp_wtrie_node_store(cp_wtrie *grp, 
                       cp_wtrie_node *node, 
					   wchar_t *key, 
					   void *leaf) 
{ 

	wchar_t *next;
	wtab_node *map_node;
	cp_wtrie_node *sub;

	map_node = WNODE_MATCH(node, key); 

	if (map_node == NULL) /* not mapped - just add it */
	{ 
		sub = cp_wtrie_node_new(leaf, grp->mempool); 
		if (WNODE_STORE(node, key, sub) == NULL) return -1; 
	} 
	else
	{
		next = map_node->attr;
		while (*key && *key == *next) 
		{
			key++;
			next++;
		}
		
		if (*next) /* branch abc, key abx or ab */
		{
			cp_wtrie_node *old = map_node->value;
			if ((sub = cp_wtrie_node_new(NULL, grp->mempool)) == NULL) return -1;
			if (WNODE_STORE(sub, next, old) == NULL) return -1;
			*next = '\0'; //~~ truncate and realloc - prevent dangling key
			map_node->value = sub;
			if (*key) /* key abx */
			{
				if ((WNODE_STORE(sub, key, 
								cp_wtrie_node_new(leaf, grp->mempool)) == NULL))
					return -1;
			}
			else /* key ab */
				sub->leaf = leaf;
		}
		else if (*key) /* branch abc, key abcde */
			return cp_wtrie_node_store(grp, map_node->value, key, leaf);

		else  /* branch abc, key abc */
		{
			cp_wtrie_node *node = ((cp_wtrie_node *) map_node->value);
			if (node->leaf && grp->delete_leaf && 
				grp->mode & COLLECTION_MODE_DEEP)
				(*grp->delete_leaf)(node->leaf);
			node->leaf = leaf;
		}
	}
	return 0;
} 

/*
 * wrapper for the recursive insertion - implements collection mode setting
 */
int cp_wtrie_add(cp_wtrie *grp, wchar_t *key, void *leaf) 
{ 
	int rc = 0; 
	if ((rc = cp_wtrie_txlock(grp, COLLECTION_LOCK_WRITE))) return rc; 

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
		if ((rc = cp_wtrie_node_store(grp, grp->root, key, leaf)))
			goto DONE;
	} 

	grp->path_count++; 

DONE:
	cp_wtrie_txunlock(grp);
	return rc; 
} 

/* helper functions for cp_wtrie_remove */

static wchar_t *mergewcs(wchar_t *s1, wchar_t *s2)
{
	wchar_t *s;
	int len = wcslen(s1) + wcslen(s2);

	s = (wchar_t *) malloc((len + 1) * sizeof(wchar_t));
	if (s == NULL) return NULL;
	
	wcscpy(s, s1); 
	wcscat(s, s2);

	return s;
}

static wtab_node *get_single_entry(wtab *t)
{
	int i;

	for (i = 0; i < t->size; i++)
		if (t->table[i]) return t->table[i];

	return NULL;
}

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
int cp_wtrie_remove(cp_wtrie *grp, wchar_t *key, void **leaf)
{
	int rc = 0;
	cp_wtrie_node *link = grp->root; 
	cp_wtrie_node *prev = NULL;
	wchar_t ccurr, cprev = 0;
	wtab_node *map_node;
	wchar_t *branch;

	if (link == NULL) return 0; /* tree is empty */

	if ((rc = cp_wtrie_txlock(grp, COLLECTION_LOCK_READ))) return rc;
 
	if (key == NULL) /* as a special case, NULL keys are stored on the root */
	{
		if (link->leaf)
		{
			grp->path_count--;
			link->leaf = NULL;
		}
		goto DONE;
	}

	/* keep pointers one and two nodes up for merging in cases (2), (3) */
	ccurr = *key;
	while ((map_node = WNODE_MATCH(link, key)) != NULL) 
	{
		branch = map_node->attr;
 
		while (*key && *key == *branch)
		{
			key++;
			branch++;
		}
		if (*branch) break; /* mismatch */
		if (*key == '\0') /* found */
		{
			wchar_t *attr;
			cp_wtrie_node *node = map_node->value;
			if (leaf) *leaf = node->leaf;
			if (node->leaf)
			{
				grp->path_count--;
				rc = 1;
				/* unlink leaf */
        		if (grp->delete_leaf && grp->mode & COLLECTION_MODE_DEEP)
                	(*grp->delete_leaf)(node->leaf);
				node->leaf = NULL;

				/* now remove node - case (1) */
				if (wtab_count(node->others) == 0) 
				{
					wtab_remove(link->others, ccurr);
					cp_wtrie_node_unmap(grp, &node);
					
					/* case (2) */
					if (prev &&
						wtab_count(link->others) == 1 && link->leaf == NULL)
					{
						wtab_node *sub2 = wtab_get(prev->others, cprev);
						wtab_node *sub = get_single_entry(link->others);
						attr = mergewcs(sub2->attr, sub->attr);
						if (attr)
						{
							if (WNODE_STORE(prev, attr, sub->value))
							{
								wtab_remove(link->others, sub->key);
								cp_wtrie_node_delete(grp, link);
							}
							free(attr);
						}
					}
				}
				else if (wtab_count(node->others) == 1) /* case (3) */
				{
					wtab_node *sub = get_single_entry(node->others);
					attr = mergewcs(map_node->attr, sub->attr);
					if (attr)
					{
						if (WNODE_STORE(link, attr, sub->value))
						{
							wtab_remove(node->others, sub->key);
							cp_wtrie_node_delete(grp, node);
						}
						free(attr);
					}
				}
			}
			break;
		}
		
		prev = link;
		cprev = ccurr;
		ccurr = *key;
		link = map_node->value; 
	} 

DONE: 
	cp_wtrie_txunlock(grp);
	return rc;
}

void *cp_wtrie_exact_match(cp_wtrie *grp, wchar_t *key)
{
	void *last = NULL;
	cp_wtrie_node *link = grp->root; 
	wtab_node *map_node;
	wchar_t *branch = NULL;

	if (cp_wtrie_txlock(grp, COLLECTION_LOCK_READ)) return NULL; 
 
	while ((map_node = WNODE_MATCH(link, key)) != NULL) 
	{
		branch = map_node->attr;
 
		while (*key && *key == *branch)
		{
			key++;
			branch++;
		}
		if (*branch) break; /* mismatch */

		link = map_node->value; 
	} 

	if (link) last = link->leaf;

	cp_wtrie_txunlock(grp);
 
	if (*key == '\0' && branch && *branch == '\0') 
		return last;  /* prevent match on super-string keys */
	return NULL;
}

/* return a vector containing exact match and any prefix matches */
cp_vector *cp_wtrie_fetch_matches(cp_wtrie *grp, wchar_t *key)
{
	int rc;
	cp_vector *res = NULL;
	cp_wtrie_node *link = grp->root;
	wtab_node *map_node;
	wchar_t *branch;

	if ((rc = cp_wtrie_txlock(grp, COLLECTION_LOCK_READ))) return NULL; 
 
	while ((map_node = WNODE_MATCH(link, key)) != NULL) 
	{
		branch = map_node->attr;

		while (*key && *key == *branch)
		{
			key++;
			branch++;
		}
		if (*branch) break; /* mismatch */
	
		link = map_node->value; 
		if (link->leaf)
		{ 
			if (res == NULL)
			{
				res = cp_vector_create(1);
				if (res == NULL) break;
			}
			cp_vector_add_element(res, link->leaf);
		} 
	}

	cp_wtrie_txunlock(grp);
	return res; 
} 

static void extract_subwtrie(cp_wtrie_node *link, cp_vector *v);

static int extract_node(void *n, void *v)
{
	wtab_node *node = n;

	extract_subwtrie(node->value, v);

	return 0;
}

static void extract_subwtrie(cp_wtrie_node *link, cp_vector *v)
{
	if (link->leaf)
		cp_vector_add_element(v, link->leaf);

	if (link->others)
		wtab_callback(link->others, extract_node, v);
}

/* return a vector containing all enwtries in subtree under path given by key */
cp_vector *cp_wtrie_submatch(cp_wtrie *grp, wchar_t *key)
{
	int rc;
	cp_vector *res = NULL;
	cp_wtrie_node *link = grp->root;
	wtab_node *map_node;
	wchar_t *branch;

	if ((rc = cp_wtrie_txlock(grp, COLLECTION_LOCK_READ))) return NULL; 
 
	while ((map_node = WNODE_MATCH(link, key)) != NULL) 
	{
		branch = map_node->attr;

		while (*key && *key == *branch)
		{
			key++;
			branch++;
		}

		if (*branch && *key) break; /* mismatch */
	
		link = map_node->value; 

		if (*key == '\0')
		{
			res = cp_vector_create(1);
			extract_subwtrie(link, res);
			break;
		}
	}

	cp_wtrie_txunlock(grp);
	return res; 
} 

/* return longest prefix match for given key */
int cp_wtrie_prefix_match(cp_wtrie *grp, wchar_t *key, void **leaf)
{ 
	int rc, match_count = 0; 
	void *last = grp->root->leaf; 
	cp_wtrie_node *link = grp->root; 
	wtab_node *map_node;
	wchar_t *branch;

	if ((rc = cp_wtrie_txlock(grp, COLLECTION_LOCK_READ))) return rc; 
 
	while ((map_node = WNODE_MATCH(link, key)) != NULL) 
	{ 
		branch = map_node->attr;
 
		while (*key && *key == *branch)
		{
			key++;
			branch++;
		}
		if (*branch) break; /* mismatch */

		link = map_node->value; 
		if (link->leaf)
		{ 
			last = link->leaf; 
			match_count++; 
		} 
	} 

	*leaf = last; 
 
	cp_wtrie_txunlock(grp);
 
	return match_count; 
} 

int cp_wtrie_count(cp_wtrie *grp)
{
	return grp->path_count;
}

void cp_wtrie_set_root(cp_wtrie *grp, void *leaf) 
{ 
    grp->root->leaf = leaf; 
} 

/* dump wtrie to stdout - for debugging */

static int node_count;
static int depth_total;
static int max_level;

void cp_wtrie_dump_node(cp_wtrie_node *node, int level, char *prefix)
{
	int i;
	wtab_node *map_node;

	node_count++;
	depth_total += level;
	if (level > max_level) max_level = level;

	for (i = 0; i < node->others->size; i++)
	{
		map_node = node->others->table[i];
		while (map_node)
		{
			cp_wtrie_dump_node(map_node->value, level + 1, map_node->attr);
			map_node = map_node->next;
		}
	}

	for (i = 0; i < level; i++) printf("\t");

	printf(" - %s => [%s]\n", prefix, node->leaf ? (char *) node->leaf : "");
}
			
void cp_wtrie_dump(cp_wtrie *grp)
{
	node_count = 0;
	depth_total = 0;
	max_level = 0;
	
	cp_wtrie_dump_node(grp->root, 0, "");

#ifdef DEBUG
	printf("\n %d nodes, %d deepest, avg. depth is %.2f\n\n", 
			node_count, max_level, (float) depth_total / node_count);
#endif
}

/* set wtrie to use given mempool or allocate a new one if pool is NULL */
int cp_wtrie_use_mempool(cp_wtrie *wtrie, cp_mempool *pool)
{
	int rc = 0;
	
	if ((rc = cp_wtrie_txlock(wtrie, COLLECTION_LOCK_WRITE))) return rc;
	
	if (pool)
	{
		if (pool->item_size < sizeof(cp_wtrie_node))
		{
			rc = EINVAL;
			goto DONE;
		}
		if (wtrie->mempool) 
		{
			if (wtrie->path_count) 
			{
				rc = ENOTEMPTY;
				goto DONE;
			}
			cp_mempool_destroy(wtrie->mempool);
		}
		cp_mempool_inc_refcount(pool);
		wtrie->mempool = pool;
	}
	else
	{
		wtrie->mempool = 
			cp_mempool_create_by_option(COLLECTION_MODE_NOSYNC, 
										sizeof(cp_wtrie_node), 0);
		if (wtrie->mempool == NULL) 
		{
			rc = ENOMEM;
			goto DONE;
		}
	}

DONE:
	cp_wtrie_txunlock(wtrie);
	return rc;
}


/* set wtrie to use a shared memory pool */
int cp_wtrie_share_mempool(cp_wtrie *wtrie, cp_shared_mempool *pool)
{
	int rc;

	if ((rc = cp_wtrie_txlock(wtrie, COLLECTION_LOCK_WRITE))) return rc;

	if (wtrie->mempool)
	{
		if (wtrie->path_count)
		{
			rc = ENOTEMPTY;
			goto DONE;
		}

		cp_mempool_destroy(wtrie->mempool);
	}

	wtrie->mempool = cp_shared_mempool_register(pool, sizeof(cp_wtrie_node));
	if (wtrie->mempool == NULL) 
	{
		rc = ENOMEM;
		goto DONE;
	}
	
DONE:
	cp_wtrie_txunlock(wtrie);
	return rc;
}


