
#ifndef BF_LINK_H_
#define BF_LINK_H_

// linked list
struct bf_link {
	void *data;
	struct bf_link *next;
};

typedef struct bf_link bf_link;

bf_link *bf_link_push(bf_link *lnk, void *data);
void *bf_link_pop(bf_link *lnk);
void *bf_link_pop_bottom(bf_link *lnk);
void *bf_link_top(bf_link *lnk);
void *bf_link_bottom(bf_link *lnk);
bool bf_link_empty(bf_link *lnk);

#endif // BF_LINK_H_
