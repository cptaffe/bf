
#ifndef BF_ASTREE_H_
#define BF_ASTREE_H_

// aspects of the tree
typedef enum {
	BF_ASTREE_ROOT,
	BF_ASTREE_LOOP,
	BF_ASTREE_ID,
} bf_astree_type_t;

// recursive tree structure
struct bf_astree {
	// data
	bf_astree_type_t type;
	void *data;

	// children
	int chld_num;
	struct bf_astree **chld;
};

typedef struct bf_astree bf_astree;

bf_astree *bf_astree_init_root();
bf_astree *bf_astree_init(bf_astree_type_t type, void *data);
void bf_astree_free(bf_astree* t);
int bf_astree_child_add(bf_astree *t, bf_astree *c);
int bf_astree_child_rm(bf_astree *t, int index);

#endif // BF_ASTREE_H_
