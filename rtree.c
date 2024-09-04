#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    int64_t x1,x2,y1,y2;
} Box;

#define MAXFILL 16

typedef struct _RTree {
    int n;
    Box boxes[MAXFILL];
    struct _RTree *chldrn[MAXFILL];
} RTree;

int box_cross(Box a, Box b){
    return a.x1 <= b.x2 && a.x2 >= b.x1 &&
           a.y1 <= b.y2 && a.y2 >= b.y1;
}

#define MAX(a, b) a>b?a:b
#define MIN(a, b) a<b?a:b
#define ABS(a) (a<0?-(a):a)

Box box_grow(Box a, Box b){
    return (Box){
        MIN(a.x1, b.x1),
        MAX(a.x2, b.x2),
        MIN(a.y1, b.y1),
        MAX(a.y2, b.y2),
    };
}

int64_t box_area(Box b){
    return (b.x2 - b.x1) * (b.y2 - b.y1);
}
int64_t box_exp(Box a, Box b){
    return box_area(box_grow(a, b)) - box_area(a);
}

Box *rt_search(RTree *t, Box *b){
    for(int i=0; i<t->n; i++){
        if(!box_cross(t->boxes[i], *b))
            continue;
        if(!t->chldrn[i])
            return &t->boxes[i];
        Box *res = rt_search(t->chldrn[i], b);
        if(res)
            return res;
    }
    return NULL;
}

void rt_split(RTree *t){
    int64_t exp = -1;
    int a, b;
    for(int i=0; i<MAXFILL; i++){
        for(int j=i+1; i<MAXFILL; i++){
            int64_t cmp = box_area(box_grow(t->boxes[i], t->boxes[j]));
            if(cmp > exp)
                exp = cmp, a=i, b=j;
        }
    }
    int pair[2] = {a,b};

    RTree r = {.n=2};
    for(int i=0; i<2; i++){
        r.boxes[i] = t->boxes[pair[i]];
        r.chldrn[i] = malloc(sizeof(RTree));
        r.chldrn[i]->n = 0;
    }
    for(int i=0; i<MAXFILL; i++){
        int s = 0;
        int64_t c0 = box_exp(t->boxes[pair[0]], t->boxes[i]);
        int64_t c1 = box_exp(t->boxes[pair[1]], t->boxes[i]);
        if(c1 < c0) s = 1;
        int p = r.chldrn[s]->n++;
        r.chldrn[s]->boxes[p] = t->boxes[i];
        r.chldrn[s]->chldrn[p] = t->chldrn[i];
        r.boxes[s] = box_grow(r.boxes[s], t->boxes[i]);
    }
    *t = r;
}

int _rt_insert(RTree *t, Box *b){
    if(!t->chldrn[0]){
        if(t->n == MAXFILL) *(volatile int*)0;
        t->boxes[t->n++] = *b;
        return t->n < MAXFILL;
    }
    int64_t area = box_exp(t->boxes[0], *b);
    int idx = 0;
    for(int i=1; i<t->n; i++){
        int64_t cmp = box_exp(t->boxes[i], *b);
        if(cmp < area){
            area = cmp;
            idx = i;
        }
    }
    t->boxes[idx] = box_grow(t->boxes[idx], *b);
    if(_rt_insert(t->chldrn[idx], b))
        return 1;
    if(t->n == MAXFILL) *(volatile int*)0;
    RTree *c = t->chldrn[idx];
    rt_split(c);
    t->chldrn[idx] = c->chldrn[0];
    t->boxes[idx] = c->boxes[0];
    t->chldrn[t->n] = c->chldrn[1];
    t->boxes[t->n] = c->boxes[1];
    printf("split %p > %p %p\n", t, c->chldrn[0], c->chldrn[1]);
    free(c);
    return ++t->n != MAXFILL;
}

void rt_insert(RTree *t, Box *b){
    if(_rt_insert(t, b))
        return;
    printf("split %p\n", t);
    rt_split(t);
}

void rt_print(RTree *t, int n){
    char *spc = "                               ";
    for(int i=0; i<t->n; i++){
        printf("%.*s %ld %ld %ld %ld %p\n", n*3, spc, t->boxes[i].x1, t->boxes[i].y1, t->boxes[i].x2, t->boxes[i].y2, t->chldrn[i]);
        if(t->chldrn[i])
            rt_print(t->chldrn[i], n+1);
    }
}

int main(int argc, char **argv){
    FILE *fp = stdin;
    if(argc>1) fp = fopen(argv[1], "r");
    int64_t x=0, y=0;
    RTree t = {0};
    int tot;
    int64_t dist = 0;
    fscanf(fp, "%d\n", &tot);
    for(int i=0; i<tot; i++){
        char c;
        int num;
        fscanf(fp, "%c %d\n", &c, &num);
        int xp = x, yp = y;
        if(c=='U') y+=num-1;
        if(c=='D') y-=num-1;
        if(c=='L') x+=num-1;
        if(c=='R') x-=num-1;
        Box b = (Box){
            .x1=MIN(xp,x),
            .y1=MIN(yp,y),
            .x2=MAX(xp,x),
            .y2=MAX(yp,y),
        };
        if(c=='U') y+=1;
        if(c=='D') y-=1;
        if(c=='L') x+=1;
        if(c=='R') x-=1;
        int64_t least=-1;
        while(1){
            Box *f = rt_search(&t, &b);
            if(!f)
                break;
            printf("collide %ld %ld %ld %ld\n", f->x1, f->y1, f->x2, f->y2);
            int64_t d;
            if(c=='U') d = ABS(f->y1 - yp);
            if(c=='D') d = ABS(f->y2 - yp);
            if(c=='L') d = ABS(f->x1 - xp);
            if(c=='R') d = ABS(f->x2 - xp);
            least = MIN((uint64_t)least, d);
            printf("%ld\n", least);
            *f = (Box){INT64_MAX, INT64_MAX, INT64_MAX, INT64_MAX};
        }
        if(least >= 0){
            dist += least;
            break;
        }
        dist += num;
        rt_insert(&t, &b);
        //rt_print(&t, 0); printf("xxx\n");
    }
    printf("%ld\n", dist);
}
