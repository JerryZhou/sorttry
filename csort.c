#include <stdio.h>
#include <stdlib.h>

#define _cunused(v) (void)v;

#define _cpt printf
// get current system time in nanos 
#include <sys/time.h>
int64_t ccgetcurnano() {
    struct timeval tv;
    
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000*1000 + tv.tv_usec;
}

//################################################
//################################################

static void _csort_samp(long arr[], int len) {
    int i=0;
    for (; i < len; ++i) {
        arr[i] = rand() % len;
    }
    switch (len) {
        case 6:
            arr[5] = 7;
        case 5:
            arr[4] = 3;
        case 4:
            arr[3] = 9;
        case 3:
            arr[2] = 1;
        case 2:
            arr[1] = 5;
        case 1:
            arr[0] = 8;
        default:
            break;
    }
}

static void _csort_cpy(long arr[], int begin, int end, long source[], int srcbegin) {
    for (; begin<end;) {
        arr[begin++] = source[srcbegin++];
    }
}

// smart print array
static void _csort_print_arr(long arr[], int start, int end, const char *divide) {
    int cneedshortbefore = 3;
    int cneedshortafter = 5;

    int before = cneedshortbefore;
    int left = 0;

    if (divide) {
        printf("%s", divide);
    }
    for (; start<=end; ++start) {
        printf("arr[%d] = %ld%s", start, arr[start], (start==end) ? "\n" : ", ");

        --before;
        if (before == 0) {
            left = end - start + 1;
            if (left > cneedshortafter) {
                start = end - cneedshortafter + 1;
                before = cneedshortafter; 
                printf("..., ");
            }
        }
    }
}

static void _csort_print(long arr[], int len) {
    _csort_print_arr(arr , 0, len-1, NULL);
}

static int _csort_compare_xx_cnt = 0;
static int _csort_compare(long left, long right) {
    ++_csort_compare_xx_cnt;
    return (int)(left - right);
}

static void _csort_swap(long arr[], int lfs, int rfs) {
    long tmp = arr[lfs];
    arr[lfs] = arr[rfs];
    arr[rfs] = tmp;
}

static void _csort_move(long arr[], int start, int len, int offset) {
    int end = start + len - 1;
    if (offset < 0) {
        for (; start <= end; ++start) {
            arr[start+offset] = arr[start];
        }
    } else {
        for (; end >= start; --end) {
            arr[end+offset] = arr[end];
        }
    }
}

//################################################
//################################################
// > 0 --> left > right ;
// ==0 --> left == right;
// < 0 --> left < right;
typedef int (*csort_compare_fn)(long left, long right);

//################################################
//################################################
// insert sort algorithm
// 插入排序：
// 第一个元素为初始排序号的元素
// 第二个元素开始为需要插入的元素
//  在排好序列的元素里面从后往前找，
//  比当前要插入的元素小的元素都往后移动，
//  直到找到自己的插入位置
void csort_insert(long arr[],
                  int start,
                  int end,
                  int gap,
                  csort_compare_fn compare) {
    int i = 0;
    long tmp = 0;
    int j = 0;
    for ( i = start + gap; i <= end; i += gap) {
        tmp = arr[i];
        for ( j = i - gap; j >= start && compare(arr[j], tmp) > 0; j -= gap) {
            arr[j+gap] = arr[j];
        }
        arr[j+gap] = tmp;
    }
}

//################################################
//################################################
// insert optimal sort algorithm
// 和插入排序一样，
//  只是查找位置的时候用二分查找，
//  找到插入位置后，
//  当前位置以前的元素整体往后移动一个
void csort_insertmove(long arr[],
                  int start,
                  int end,
                  csort_compare_fn compare) {
    int i = 0;
    long tmp = 0;

    int n = 0;
    int m = 0;
    int k = 0;
    int cmp = 0;
    for (i = start + 1; i <= end; i += 1) {
        tmp = arr[i];

        n = start;
        m = i-1;
        k = 0;
        for (;n <= m;) {
            k = (n + m)/2;
            cmp = compare(arr[k], tmp); 
            if (m == n) {
                if (cmp > 0) {
                } else {
                    k = k+1;
                }
                break;
            }
            if ( cmp > 0 ) {
                m = k-1;
            } else {
                n = k+1;
            } 
        }
        if (k < i) {
            _csort_move(arr, k, i-k, 1);
        }
        arr[k] = tmp;
    }
}

//################################################
//################################################
// selection sort algorithm
// 选择排序：
//  查找列表里面最小元素的位置，和列表的第一个进行交换, 
//  然后游标后移，查找剩下列表里面的最小元素, 再一次交换
void csort_selection(long arr[],
                     int start,
                     int end,
                     csort_compare_fn compare) {
    int i, j;
    int min;
    long tmp;
    for (i=start; i<end; ++i) {
        min = i;
        for (j=i+1; j<=end; ++j) {
            if(compare(arr[min], arr[j]) > 0) {
                min = j;
            }
        }
        if (min != i) {
            tmp = arr[min];
            arr[min] = arr[i];
            arr[i] = tmp;
        }
    }
}

//################################################
//################################################
// shell sort algorithm
// 希尔排序：
//  插入排序的升级版
//      具备有序，插入间隔逐渐缩小，直到插入间隔变为1
void csort_shell(long arr[],
                 int start,
                 int end,
                 csort_compare_fn compare) {
    int gap;
    
    for (gap = (end - start + 1) >> 1; gap > 0 ; gap >>= 1) {
        csort_insert(arr, start, end, gap, compare);
    }
}

//################################################
//################################################
// bubble sort algorithm
// 冒泡排序：
//  遍历列表，两两比较元素，如果元素后面的小，则交换，
//  这样一次偏离就把最大的元素挪动到了最后，
//  如果一次遍历里面没有发生交换，说明以及排好序了
void csort_bubble(long arr[],
                  int start,
                  int end,
                  csort_compare_fn compare) {
    int i, j;
    int bub;
    long tmp;
    for (i=start; i<=end-1; ++i) {
        bub = 0;
        for (j=start; j<=end-1-i+start; ++j) {
            if (compare(arr[j], arr[j+1]) > 0) {
                tmp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = tmp;
                bub++;
            }
        }
        if (bub == 0) {
            break;
        }
    }
}

//################################################
//################################################


//static void _csort_reverse(long arr[], int start, int len) {
//    int end = start + len -1;
//    for (; start<end; ++start,--end) {
//        _csort_swap(arr, start, end);
//    }
//}

static void _csort_merge_internal(long arr[],
                                  int start,
                                  int end,
                                  int mid,
                                  csort_compare_fn compare) {
    
    int i,j,ik,jk;
    int cmp;
    long tmp;
    
    i = start;
    j = mid + 1;
    ik = 0;
    
    for (; i<=mid && j<=end; ) {
        if (ik > 0) {
            cmp = compare(arr[j-ik], arr[j]);
        } else {
            cmp = compare(arr[i], arr[j]);
        }
        if (cmp > 0) {
            _csort_swap(arr, i, j);
            ++j;
            ++i;
            ++ik;
        } else {
            if (ik > 0) {
                _csort_swap(arr, i, j-ik);
                if (ik > 1) {
                    tmp = arr[j-ik];
                    _csort_move(arr, j-ik+1, ik-1, -1);
                    arr[j-1] = tmp;
                }
                ++i;
            } else {
                ++i;
            }
        }
        if (i > mid) {
            if (ik == 0) {
                break;
            } else {
                mid = j -1;
                ik = 0;
            }
        }
    }
    
    if (ik > 0) {
        jk = j - ik;
        for (; mid>=i; --mid) {
            tmp = arr[mid];
            _csort_move(arr, jk, ik, -1);
            arr[jk + ik - 1] = tmp;
            --jk;
        }
    }
}

static int csort_depth = 0;
static const char* sortsdeptchsdivide[] = {"",
    "\t",
    "\t\t",
    "\t\t\t",
    "\t\t\t\t",
    "\t\t\t\t\t",
    "\t\t\t\t\t\t",
    "\t\t\t\t\t\t\t",
    "\t\t\t\t\t\t\t\t",
    "\t\t\t\t\t\t\t\t\t",
    };
static int CDebug = 0;
// 归并排序：
//  合并两个有序的列表
//      把列表一分为二，然后分别对列表进行排序，排好序后，进行归并
//      这里改进的地方在原地归并，不增加额外空间
void csort_merge(long arr[],
                 int start,
                 int end,
                 csort_compare_fn compare) {
    int mid = 0;
    ++csort_depth;
    
    if (CDebug) {
        printf("%s----begin csort_merge arr[%d:%d]:\n", sortsdeptchsdivide[csort_depth], start, end);
        _csort_print_arr(arr, start, end, sortsdeptchsdivide[csort_depth]);
    }
    
    if (start < end) {
        mid = (start + end)/2;
        csort_merge(arr, start, mid, compare);
        csort_merge(arr, mid+1, end, compare);
        _csort_merge_internal(arr, start, end, mid, compare);
    }
    
    if (CDebug) {
        _csort_print_arr(arr, start, end, sortsdeptchsdivide[csort_depth]);
        printf("%s----end csort_merge arr[%d:%d]:\n", sortsdeptchsdivide[csort_depth], start, end);
    }
    
    --csort_depth;
}

//################################################
//################################################
// i  --> (left, right) [2*i+1, 2*i+2]
// 堆排序：
//  先构建堆
//  然后取堆顶元素，堆定元素和最后一个叶子节点交换
//  然后缩小堆的大小，重新构建一个堆
// 构建堆： 
//  先从最后一个非叶子节点开始, 
//  依次加入元素, 
//  调整堆顶元素
void csort_heap_shift(long arr[], 
        int ind, 
        int end, 
        csort_compare_fn compare) {

    int i = ind;
    int c = 2 * i + 1;
    long tmp;

    while(c <= end) {
        if (c +1 <=end && compare(arr[c], arr[c+1]) < 0 ) {
            c++;
        }
        if (compare(arr[i], arr[c]) > 0) {
            break;
        } else {
            tmp = arr[i];
            arr[i] = arr[c];
            arr[c] = tmp;

            i = c;
            c = 2*i + 1;
        }
    }

}

void csort_heap(long arr[],
                 int start,
                 int end,
                 csort_compare_fn compare) {
    int i, j;
    long tmp;
    for (i=(end-1)/2; i>=start; i--) {
        csort_heap_shift(arr, i, end, compare);
    } 

    for (j=start; j<=end; ++j) {
        tmp = arr[start];
        arr[start] = arr[end-start-j];
        arr[end-start-j] = tmp; 

        csort_heap_shift(arr, start, end - start - j - 1, compare);
    }
}

//################################################
//################################################
// 开速排序：
//  选一个中间元素，把所有元素分成两部分
//  左变是小的，右边是大的
//  然后一次对左右两边执行同样的算法
int csort_quick_partition(long arr[], 
        int left, 
        int right, 
        csort_compare_fn compare) {

    int smallindex = left;
    long pivot = arr[smallindex];
    int i;

    _csort_swap(arr, smallindex, right); 
    for (i=left; i<right; ++i) {
        if (compare(arr[i], pivot) < 0 ) {
            _csort_swap(arr, i, smallindex);
            ++smallindex;
        }
    }
    _csort_swap(arr, smallindex, right); 

    return smallindex;
}

#include <stdio.h>
#include <stdlib.h>
void csort_quick(long arr[],
        int start,
        int end,
        csort_compare_fn compare) {
    int priotindex;
    if (start >= end) {
        return;
    }
    priotindex = csort_quick_partition(arr, start, end, compare); 
    csort_quick(arr, start, priotindex, compare);
    csort_quick(arr, priotindex+1, end, compare);
}

//################################################
//################################################
//http://www.sorting-algorithms.com/static/QuicksortIsOptimal.pdf
// 快速排序的改进版本：
//  把所有元素分词三部分，
//      左边的小，
//      中间的相等，
//      右边的大
void csort_quick3(long arr[], int start, int end, csort_compare_fn compare) {
    int i = start - 1, j = end, p = start-1, q = end, k=start;
    long v = arr[end];
    if (end<=start) return;
    for (;;) {
        while (compare(arr[++i], v) < 0) ;
        while (compare(v, arr[--j]) < 0) if (j==start) break;
        if (i >= j) break;
        _csort_swap(arr, i, j);
        if (compare(arr[i], v) == 0) { p++; _csort_swap(arr, p, i); }
        if (compare(arr[j], v) == 0) { q--; _csort_swap(arr, q, j); }
    }
    _csort_swap(arr, i, end); j = i - 1; i = i + 1;
    for (k=start; k<p; ++k, --j) { _csort_swap(arr, k, j); }
    for (k=end-1; k>q; --k, ++i) { _csort_swap(arr, i, k); }
    csort_quick3(arr, start, j, compare);
    csort_quick3(arr, i, end, compare);
}

//################################################
//################################################
struct SortTestContext;
// sort function, return the compare
typedef void (*csort_fn)(long arr[], int len, csort_compare_fn compare);

// sort run once try struct
typedef struct SortTestTry {
    struct SortTestContext *context;
    const char* name;
    csort_fn sortfn;
    int index;

    int len;
    long *arr;

    int compares;
    int64_t nanos;
}SortTestTry;

// sort algorithms
typedef enum CSortEnum {
    CSort_Insert,
    CSort_InsertMove,
    CSort_Shell,
    CSort_Bubble,
    CSort_Selection,
    CSort_Merge,
    CSort_Heap,
    CSort_Quick,
    CSort_Quick3,

    CSort_Max,
}CSortEnum;

// shared sort context
typedef struct SortTestContext {
    long *source;
    int len;
    csort_compare_fn comparefn;
    struct SortTestTry *allSorts[CSort_Max];
}SortTestContext;

// add sort test try
void addSortTry(SortTestContext *context, const char* name, 
        csort_fn fn, 
        int index
        ) {
    SortTestTry * sorttry  = (SortTestTry *)(calloc(1, sizeof(SortTestTry)));
    sorttry->name = name;
    sorttry->sortfn = fn;
    sorttry->context = context;
    sorttry->index = index;
    context->allSorts[index] = sorttry;
}

// sort the array in context: will copy the array
void csort_execute(SortTestTry* sorttry) {
    int len = sorttry->context->len;
    long *arr = NULL; 
    if(sorttry->arr == NULL || sorttry->len < len) {
        free(sorttry->arr);
        sorttry->arr =  (long*)malloc(sizeof(long)*len);
    } 
    arr = sorttry->arr;

    // copy the sort sample form source
    _csort_cpy(arr, 0, len, sorttry->context->source, 0);
    // sort 
    _csort_compare_xx_cnt = 0;
    sorttry->nanos = ccgetcurnano();
    sorttry->sortfn(arr, len, sorttry->context->comparefn); 
    sorttry->nanos = ccgetcurnano() - sorttry->nanos;
    sorttry->compares = _csort_compare_xx_cnt;

    printf("##### [%s] [Array Size %d] [compares %d] [nanos %lld]\n", 
            sorttry->name, len, sorttry->compares, sorttry->nanos);
}

// run one sort
void csort_context_run(SortTestContext* context, int index) {
    SortTestTry *sorttry = context->allSorts[index];
    if (sorttry) {
        csort_execute(sorttry);
    }
}

// run sort in context
void csort_context_runonce(SortTestContext *context) {
    int sortindex;

    // foreach the sorttry
    for (sortindex=0; sortindex<CSort_Max; ++sortindex) {
        csort_context_run(context, sortindex);
    }
}


// if the sort result is same
int csort_compare_result_same(SortTestTry* lfs, SortTestTry* rfs) {
    int len;
    int same = 0;
    if (lfs == rfs) {
        return same;
    }
    if (!lfs || !rfs) {
        return -1;
    }
    if (!lfs->context || !rfs->context) {
        return -1;
    }
    if (!lfs->arr || !rfs->arr) {
        return -1;
    }
    same = lfs->context - rfs->context;
    if (same) {
        return same;
    }
    len = lfs->context->len;

    for (--len; len >= 0; --len) {
        same = lfs->arr[len] - rfs->arr[len];
        if (same) {
            break;
        }
    }

    return same;
}

// run sort in context
void csort_context_checkresult(SortTestContext *context) {
    int sortindex;
    SortTestTry *lfs, *rfs;

    // foreach the sorttry
    for (sortindex=1; sortindex<CSort_Max; ++sortindex) {
        lfs = context->allSorts[sortindex-1];
        rfs = context->allSorts[sortindex];
        if (csort_compare_result_same(lfs, rfs) != 0) {
            printf("[Error] The Sort Result Is Conflict\n");
            printf("[Error] Origin:\n");
            _csort_print(context->source, context->len);

            printf("[Error] Left Algorithm ##### [%s] [Array Size %d] [compares %d]\n", 
            lfs->name, context->len, lfs->compares);
            _csort_print(lfs->arr, context->len);

            printf("[Error] Right Algorithm ##### [%s] [Array Size %d] [compares %d]\n", 
            rfs->name, context->len, rfs->compares);
            _csort_print(rfs->arr, context->len);
        }
    }
}

// Insert Sort Try
static void _csort_fn_insert(long arr[], int len, csort_compare_fn compare) {
    csort_insert(arr, 0, len-1, 1, compare);
}

// InsertMove Sort Try
static void _csort_fn_insertmove(long arr[], int len, csort_compare_fn compare) {
    csort_insertmove(arr, 0, len-1, compare);
}

// Shell Sort Try
static void _csort_fn_shell(long arr[], int len, csort_compare_fn compare) {
    csort_shell(arr, 0, len-1, compare);
} 

// Bubble Sort Try
static void _csort_fn_bubble(long arr[], int len, csort_compare_fn compare) {
    csort_bubble(arr, 0, len-1, compare);
}

// Selection Sort Try
static void _csort_fn_selection(long arr[], int len, csort_compare_fn compare) {
    csort_bubble(arr, 0, len-1, compare);
}

// Merge Sort Try
static void _csort_fn_merge(long arr[], int len, csort_compare_fn compare) {
    csort_merge(arr, 0, len-1, compare);
}

// Heap Sort Try
static void _csort_fn_heap(long arr[], int len, csort_compare_fn compare) {
    csort_heap(arr, 0, len-1, compare);
} 

// Quick Sort Try
static void _csort_fn_quick(long arr[], int len, csort_compare_fn compare) {
    csort_quick(arr, 0, len-1, compare);
}

// 3 Way Quick Sort Try
static void _csort_fn_quick3(long arr[], int len, csort_compare_fn compare) {
    csort_quick3(arr, 0, len-1, compare);
}

// Setup It
void setupAllSortTrys(SortTestContext *context) {
    addSortTry(context, "insert", _csort_fn_insert, CSort_Insert);
    addSortTry(context, "insertmove", _csort_fn_insertmove, CSort_InsertMove);
    addSortTry(context, "shell", _csort_fn_shell, CSort_Shell);
    addSortTry(context, "bubble", _csort_fn_bubble, CSort_Bubble);
    addSortTry(context, "selection", _csort_fn_selection, CSort_Selection);
    addSortTry(context, "merge", _csort_fn_merge, CSort_Merge);
    addSortTry(context, "heap", _csort_fn_heap, CSort_Heap);
    addSortTry(context, "quick", _csort_fn_quick, CSort_Quick);
    addSortTry(context, "quick3", _csort_fn_quick3, CSort_Quick3);
}

// release all the resource that holded by context
void releaseSortTryContext(SortTestContext *context) {
    int sortindex;
    SortTestTry *sorttry;

    // free source
    free(context->source);
    context->source = NULL;

    // foreach the sorttry
    for (sortindex=0; sortindex<CSort_Max; ++sortindex) {
        sorttry = context->allSorts[sortindex];
        if (sorttry) {
            free(sorttry->arr);
            free(sorttry);
            context->allSorts[sortindex] = NULL;
        }
    }
}

static int const CLen = 3500;

// test the sort trys
void testSortTrys(int num, int len) {
    int trynum = num;
    int maxlen = len;

    // setup context
    SortTestContext context;
    context.len = len;
    context.source = (long*)malloc(sizeof(long)*context.len);
    context.comparefn = _csort_compare;
    setupAllSortTrys(&context);

    for (; trynum >= 0; --trynum) {
        printf("*******************************************************\n");
        printf("************************begin sortting[%d]**********\n", trynum);
        context.len = maxlen - rand() % maxlen;

        // make a rand sample
        _csort_samp(context.source, context.len);
        // print
        _csort_print(context.source, context.len);
        
        // foreach the sorttry
        csort_context_runonce(&context);
        // foreach check result
        csort_context_checkresult(&context);

        printf("[sortting result]:\n");
        _csort_print(context.allSorts[0]->arr, context.len);
    }

    // release context
    releaseSortTryContext(&context);
}

void testSimpleSort() {
    long arr[] = { 6, 2, 9, 4, 8};
    int cnt = sizeof(arr)/sizeof(arr[0]);

    _csort_print(arr, cnt);
    csort_insertmove(arr, 0, cnt-1, _csort_compare);
    _csort_print(arr, cnt);
}

//################################################
//################################################
int main(int argc, const char* argv[]) {

    _cunused(argc);
    _cunused(argv);

    testSortTrys(10, 100000);
      
    return 0;
}

