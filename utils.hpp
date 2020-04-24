#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// UNIX heasders
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define ASSERT_ALWAYS(x)                                                       \
  {                                                                            \
    if (!(x)) {                                                                \
      fprintf(stderr, "%s:%i [FAIL] at %s\n", __FILE__, __LINE__, #x);         \
      (void)(*(volatile int *)(NULL) = 0);                                     \
      abort();                                                                 \
    }                                                                          \
  }
#define ASSERT_DEBUG(x) ASSERT_ALWAYS(x)
#define NOTNULL(x) ASSERT_ALWAYS((x) != NULL)

#undef MIN
#undef MAX
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

template <typename T> T copy(T const &in) { return in; }

template <typename M, typename K> bool contains(M const &in, K const &key) {
  return in.find(key) != in.end();
}

#define UNIMPLEMENTED_(s)                                                      \
  {                                                                            \
    fprintf(stderr, "%s:%i UNIMPLEMENTED %s\n", __FILE__, __LINE__, s);        \
    abort();                                                                   \
  }

#define UNIMPLEMENTED UNIMPLEMENTED_("")
#define TRAP UNIMPLEMENTED_("")

template <typename F> struct __Defer__ {
  F f;
  __Defer__(F f) : f(f) {}
  ~__Defer__() { f(); }
};

template <typename F> __Defer__<F> defer_func(F f) { return __Defer__<F>(f); }

#define DEFER_1(x, y) x##y
#define DEFER_2(x, y) DEFER_1(x, y)
#define DEFER_3(x) DEFER_2(x, __COUNTER__)
#define defer(code) auto DEFER_3(_defer_) = defer_func([&]() { code; })

#define STRINGIFY(a) _STRINGIFY(a)
#define _STRINGIFY(a) #a

#define ito(N) for (uint32_t i = 0; i < N; ++i)
#define jto(N) for (uint32_t j = 0; j < N; ++j)
#define uto(N) for (uint32_t u = 0; u < N; ++u)
#define kto(N) for (uint32_t k = 0; k < N; ++k)
#define xto(N) for (uint32_t x = 0; x < N; ++x)
#define yto(N) for (uint32_t y = 0; y < N; ++y)

#define PERF_HIST_ADD(name, val)
#define PERF_ENTER(name)
#define PERF_EXIT(name)
#define OK_FALLTHROUGH (void)0;
#define TMP_STORAGE_SCOPE                                                      \
  tl_alloc_tmp_enter();                                                        \
  defer(tl_alloc_tmp_exit(););
#define SWAP(x, y)                                                             \
  {                                                                            \
    auto tmp = x;                                                              \
    x        = y;                                                              \
    y        = tmp;                                                            \
  }

static size_t get_page_size() { return sysconf(_SC_PAGE_SIZE); }

static void protect_pages(void *ptr, size_t num_pages) {
  mprotect(ptr, num_pages * get_page_size(), PROT_NONE);
}

static size_t page_align_up(size_t n) {
  return (n - get_page_size() - 1) & (~(get_page_size() - 1));
}

static size_t page_align_down(size_t n) {
  return (n) & (~(get_page_size() - 1));
}

static size_t get_num_pages(size_t size) {
  return page_align_up(size) / get_page_size();
}

static void unprotect_pages(void *ptr, size_t num_pages, bool exec = false) {
  mprotect(ptr, num_pages * get_page_size(),
           PROT_WRITE | PROT_READ | (exec ? PROT_EXEC : 0));
}

static void unmap_pages(void *ptr, size_t num_pages) {
  int err = munmap(ptr, num_pages * get_page_size());
  ASSERT_ALWAYS(err == 0);
}

static void map_pages(void *ptr, size_t num_pages) {
  void *new_ptr = mmap(ptr, num_pages * get_page_size(), PROT_READ | PROT_WRITE,
                       MAP_ANON | MAP_PRIVATE, -1, 0);
  ASSERT_ALWAYS((size_t)new_ptr == (size_t)ptr);
}

template <typename T = uint8_t> struct Temporary_Storage {
  uint8_t *ptr;
  size_t   cursor;
  size_t   capacity;
  size_t   mem_length;
  size_t   stack_capacity;
  size_t   stack_cursor;

  static Temporary_Storage create(size_t capacity) {
    ASSERT_DEBUG(capacity > 0);
    Temporary_Storage out;
    size_t            STACK_CAPACITY = 0x100 * sizeof(size_t);
    out.mem_length =
        get_num_pages(STACK_CAPACITY + capacity * sizeof(T)) * get_page_size();
    out.ptr = (uint8_t *)mmap(NULL, out.mem_length, PROT_READ | PROT_WRITE,
                              MAP_ANON | MAP_PRIVATE, -1, 0);
    ;
    out.capacity       = capacity;
    out.cursor         = 0;
    out.stack_capacity = STACK_CAPACITY;
    out.stack_cursor   = 0;
    return out;
  }

  void release() {
    munmap(this->ptr, mem_length);
    memset(this, 0, sizeof(Temporary_Storage));
  }

  void push(T const &v) {
    T *ptr = alloc(1);
    memcpy(ptr, &v, sizeof(T));
  }

  bool has_items() { return this->cursor > 0; }

  T *at(uint32_t i) {
    return (T *)(this->ptr + this->stack_capacity + i * sizeof(T));
  }

  T *alloc(size_t size) {
    ASSERT_DEBUG(size != 0);
    T *ptr = (T *)(this->ptr + this->stack_capacity + this->cursor * sizeof(T));
    this->cursor += size;
    ASSERT_DEBUG(this->cursor < this->capacity);
    return ptr;
  }

  T *alloc_page_aligned(size_t size) {
    ASSERT_DEBUG(size != 0);
    size   = page_align_up(size) + get_page_size();
    T *ptr = (T *)(this->ptr + this->stack_capacity + this->cursor * sizeof(T));
    T *aligned_ptr =
        (T *)(void *)page_align_down((size_t)ptr + get_page_size());
    this->cursor += size;
    ASSERT_DEBUG(this->cursor < this->capacity);
    return aligned_ptr;
  }

  void enter_scope() {
    // Save the cursor to the stack
    size_t *top = (size_t *)(this->ptr + this->stack_cursor);
    *top        = this->cursor;
    // Increment stack cursor
    this->stack_cursor += sizeof(size_t);
    ASSERT_DEBUG(this->stack_cursor < this->stack_capacity);
  }

  void exit_scope() {
    // Decrement stack cursor
    ASSERT_DEBUG(this->stack_cursor >= sizeof(size_t));
    this->stack_cursor -= sizeof(size_t);
    // Restore the cursor from the stack
    size_t *top  = (size_t *)(this->ptr + this->stack_cursor);
    this->cursor = *top;
  }

  void reset() {
    this->cursor       = 0;
    this->stack_cursor = 0;
  }
};

/** Allocates 'size' bytes using thread local allocator
 */
void *tl_alloc(size_t size);
/** Reallocates deleting `ptr` as a result
 */
void *tl_realloc(void *ptr, size_t oldsize, size_t newsize);
void  tl_free(void *ptr);
/** Allocates 'size' bytes using thread local temporal storage
 */
void *tl_alloc_tmp(size_t size);
/** Record the current state of thread local temporal storage
 */
void tl_alloc_tmp_enter();
/** Restore the previous state of thread local temporal storage
 */
void tl_alloc_tmp_exit();

struct string_ref {
  const char *ptr;
  size_t      len;
  string_ref  substr(size_t offset, size_t new_len) {
    return string_ref{ptr + offset, new_len};
  }
};

static bool operator==(string_ref a, string_ref b) {
  if (a.ptr == NULL || b.ptr == NULL) return false;
  return a.len != b.len ? false
                        : strncmp(a.ptr, b.ptr, a.len) == 0 ? true : false;
}

static uint64_t hash_of(uint64_t u) {
  uint64_t v = u * 3935559000370003845 + 2691343689449507681;
  v ^= v >> 21;
  v ^= v << 37;
  v ^= v >> 4;
  v *= 4768777513237032717;
  v ^= v << 20;
  v ^= v >> 41;
  v ^= v << 5;
  return v;
}

static uint64_t hash_of(string_ref a) {
  uint64_t        len         = a.len;
  uint64_t const *uint64_tptr = (uint64_t const *)a.ptr;
  uint64_t        hash        = 0;
  while (len >= 8) {
    uint64_t a = *(uint64_t *)uint64_tptr;
    hash       = hash ^ hash_of(a);
    len -= 8;
    ++uint64_tptr;
  }
  uint8_t const *u8ptr = (uint8_t *)uint64_tptr;
  switch (len) {
  case 7:
    hash = hash ^ hash_of((uint64_t)*u8ptr);
    ++u8ptr;
    OK_FALLTHROUGH
  case 6:
    hash = hash ^ hash_of((uint64_t)*u8ptr);
    ++u8ptr;
    OK_FALLTHROUGH
  case 5:
    hash = hash ^ hash_of((uint64_t)*u8ptr);
    ++u8ptr;
    OK_FALLTHROUGH
  case 4:
    hash = hash ^ hash_of((uint64_t)*u8ptr);
    ++u8ptr;
    OK_FALLTHROUGH
  case 3:
    hash = hash ^ hash_of((uint64_t)*u8ptr);
    ++u8ptr;
    OK_FALLTHROUGH
  case 2:
    hash = hash ^ hash_of((uint64_t)*u8ptr);
    ++u8ptr;
    OK_FALLTHROUGH
  case 1:
    hash = hash ^ hash_of((uint64_t)*u8ptr);
    ++u8ptr;
    OK_FALLTHROUGH
  default: break;
  }
  return hash;
}

/** String view of a static string
 */
static string_ref stref_s(char const *static_string) {
  ASSERT_DEBUG(static_string != NULL);
  string_ref out;
  out.ptr = static_string;
  out.len = strlen(static_string);
  ASSERT_DEBUG(out.len != 0);
  return out;
}

/** String view of a temporal string
  Uses thread local temporal storage
  */
static string_ref stref_tmp(char const *tmp_string) {
  ASSERT_DEBUG(tmp_string != NULL);
  string_ref out;
  out.len = strlen(tmp_string);
  ASSERT_DEBUG(out.len != 0);
  char *ptr = (char *)tl_alloc_tmp(out.len);
  memcpy(ptr, tmp_string, out.len);
  out.ptr = (char const *)ptr;

  return out;
}

static string_ref stref_concat(string_ref a, string_ref b) {
  string_ref out;
  out.len = a.len + b.len;
  ASSERT_DEBUG(out.len != 0);
  char *ptr = (char *)tl_alloc_tmp(out.len);
  memcpy(ptr, a.ptr, a.len);
  memcpy(ptr + a.len, b.ptr, b.len);
  out.ptr = (char const *)ptr;
  return out;
}

static char const *stref_to_tmp_cstr(string_ref a) {
  ASSERT_DEBUG(a.ptr != NULL);
  char *ptr = (char *)tl_alloc_tmp(a.len + 1);
  memcpy(ptr, a.ptr, a.len);
  ptr[a.len] = '\0';
  return ptr;
}

static int32_t stref_find(string_ref a, string_ref b, size_t start = 0) {
  size_t cursor = 0;
  for (size_t i = start; i < a.len; i++) {
    if (a.ptr[i] == b.ptr[cursor]) {
      cursor += 1;
    } else {
      i -= cursor;
      cursor = 0;
    }
    if (cursor == b.len) return (int32_t)(i - (cursor - 1));
  }
  return -1;
}

static int32_t stref_find_last(string_ref a, string_ref b, size_t start = 0) {
  int32_t last_pos = -1;
  int32_t cursor   = stref_find(a, b, start);
  while (cursor >= 0) {
    last_pos = cursor;
    if ((size_t)cursor + 1 < a.len)
      cursor = stref_find_last(a, b, (size_t)(cursor + 1));
  }
  return last_pos;
}

static void make_dir_recursive(string_ref path) {
  TMP_STORAGE_SCOPE;
  if (path.ptr[path.len - 1] == '/') path.len -= 1;
  int32_t sep = stref_find_last(path, stref_s("/"));
  if (sep >= 0) {
    make_dir_recursive(path.substr(0, sep));
  }
  mkdir(stref_to_tmp_cstr(path), 0777);
}

static void dump_file(char const *path, void const *data, size_t size) {
  FILE *file = fopen(path, "wb");
  ASSERT_ALWAYS(file);
  fwrite(data, 1, size, file);
  fclose(file);
}

struct Allocator {
  virtual void *    alloc(size_t)                                     = 0;
  virtual void *    realloc(void *, size_t old_size, size_t new_size) = 0;
  virtual void      free(void *)                                      = 0;
  static Allocator *get_default() {
    struct _Allocator : public Allocator {
      virtual void *alloc(size_t size) override { return tl_alloc(size); }
      virtual void *realloc(void *ptr, size_t old_size,
                            size_t new_size) override {
        return tl_realloc(ptr, old_size, new_size);
      }
      virtual void free(void *ptr) override { tl_free(ptr); }
    };
    static _Allocator alloc;
    return &alloc;
  }
};

template <typename T> struct TinyArray {
  T *    ptr;
  size_t size;
  void   init(T *ptr, size_t size) {
    this->ptr  = ptr;
    this->size = size;
  }
};

template <typename T> struct Array {
  T *        ptr;
  size_t     size;
  size_t     capacity;
  Allocator *allocator;
  size_t     grow_k;
  Array(uint32_t capacity = 0, Allocator *allocator = NULL,
        size_t grow_k = 0x100) {
    if (allocator == NULL) allocator = Allocator::get_default();
    if (capacity != 0)
      ptr = (T *)allocator->alloc(sizeof(T) * capacity);
    else
      ptr = NULL;
    size            = 0;
    this->allocator = allocator;
    this->capacity  = capacity;
    this->grow_k    = grow_k;
  }
  void release() {
    if (ptr != NULL) {
      allocator->free(ptr);
    }
    memset(this, 0, sizeof(*this));
  }
  void resize(size_t new_size) {
    if (new_size > capacity) {
      uint64_t new_capacity = new_size;
      ptr                   = (T *)allocator->realloc(ptr, sizeof(T) * capacity,
                                    sizeof(T) * new_capacity);
      capacity              = new_capacity;
    }
    ASSERT_DEBUG(capacity >= size + 1);
    ASSERT_DEBUG(ptr != NULL);
    size = new_size;
  }
  void memzero() {
    if (capacity > 0) {
      memset(ptr, 0, sizeof(T) * capacity);
    }
  }
  Array copy() {
    Array out;
    out.ptr      = NULL;
    out.size     = size;
    out.capacity = capacity;
    if (size > 0) {
      out.resize(&out, capacity);
      memcpy(out.ptr, ptr, capacity * sizeof(T));
    }
    return out;
  }
  void push(T elem) {
    if (size + 1 > capacity) {
      uint64_t new_capacity = capacity + grow_k;
      ptr                   = (T *)allocator->realloc(ptr, sizeof(T) * capacity,
                                    sizeof(T) * new_capacity);
      capacity              = new_capacity;
    }
    ASSERT_DEBUG(capacity >= size + 1);
    ASSERT_DEBUG(ptr != NULL);
    memcpy(ptr + size, &elem, sizeof(T));
    size += 1;
  }

  T pop() {
    ASSERT_DEBUG(size != 0);
    ASSERT_DEBUG(ptr != NULL);
    T elem = ptr[size - 1];
    if (size + grow_k < capacity) {
      uint64_t new_capacity = capacity - grow_k;
      ptr                   = (T *)allocator->realloc(ptr, sizeof(T) * capacity,
                                    sizeof(T) * new_capacity);
      capacity              = new_capacity;
    }
    ASSERT_DEBUG(size != 0);
    size -= 1;
    return elem;
  }
};

template <typename K, typename V> struct Hash_Pair {
  K        key;
  V        value;
  uint64_t hash;
};

template <typename K, typename V> struct HashArray {
  using HP = Hash_Pair<K, V>;
  Array<HP> arr;
  size_t    item_count;
  size_t    grow_k;
  uint32_t  attempts;

  HashArray() { memset(this, 0, sizeof(*this)); }
  void release() {
    arr.release();
    item_count = 0;
  }
  bool push(K key, V value) {
    {
      uint32_t attempts = attempts;
      uint64_t hash     = hash_of(key);
      uint64_t size     = arr.capacity;
      if (size == 0) {
        arr.resize(grow_k);
        arr.memzero();
        size = arr.capacity;
      }
      HP pair;
      pair.key            = key;
      pair.value          = value;
      uint32_t attempt_id = 0;
      for (; attempt_id < attempts; ++attempt_id) {
        uint64_t id = hash % size;
        if (hash != 0) {
          pair.hash = hash;
          if (arr.ptr[id].hash == 0) {
            memcpy(arr.ptr + id, &pair, sizeof(HP));
            item_count += 1;
            return true;
          }
        }
        hash = hash_of(hash);
      }
    }
    {
      Array<HP> old_arr = arr;
      {
        Array<HP> new_arr(0);
        new_arr.resize(&new_arr, old_arr.capacity + grow_k);
        new_arr.memzero(&new_arr);
        arr        = new_arr;
        item_count = 0;
      }
      uint32_t i = 0;
      for (; i < old_arr.capacity; ++i) {
        HP pair = old_arr.ptr[i];
        if (pair.hash != 0) {
          push(pair.key, pair.value);
        }
      }
      old_arr.release();
      bool res = push(key, value);
      ASSERT_DEBUG(res == true);
      return true;
    }
    ASSERT_DEBUG(false && "unreachable");
  }

  bool get(K key, V *value) {
    uint64_t hash = hash_of(key);
    uint64_t size = arr.capacity;
    if (size == 0) return false;
    Array<HP> *arr        = &arr;
    uint32_t   attempt_id = 0;
    for (; attempt_id < attempts; ++attempt_id) {
      uint64_t id = hash % size;
      if (hash != 0) {
        if (arr->ptr[id].key == key) {
          if (value != NULL) *value = arr->ptr[id].value;
          return true;
        }
      }
      hash = hash_of(hash);
    }
    return false;
  }

  bool has(K key) { return get(key, NULL); }
};

#endif

#ifdef UTILS_IMPL
#ifndef UTILS_IMPL_H
#define UTILS_IMPL_H
#include <string.h>

struct Thread_Local {
  Temporary_Storage<> temporal_storage;
  bool                initialized = false;
  ~Thread_Local() { temporal_storage.release(); }
};

// TODO(aschrein): Change to __thread?
thread_local Thread_Local g_tl{};

Thread_Local *get_tl() {
  if (g_tl.initialized == false) {
    g_tl.initialized      = true;
    g_tl.temporal_storage = Temporary_Storage<>::create(1 << 24);
  }
  return &g_tl;
}

void *tl_alloc_tmp(size_t size) {
  return get_tl()->temporal_storage.alloc(size);
}

void tl_alloc_tmp_enter() { get_tl()->temporal_storage.enter_scope(); }
void tl_alloc_tmp_exit() { get_tl()->temporal_storage.exit_scope(); }

void *tl_alloc(size_t size) { return malloc(size); }

void *tl_realloc(void *ptr, size_t oldsize, size_t newsize) {
  if (oldsize == newsize) return ptr;
  size_t min_size = oldsize < newsize ? oldsize : newsize;
  void * new_ptr  = NULL;
  if (newsize != 0) new_ptr = malloc(newsize);
  if (min_size != 0) {
    memcpy(new_ptr, ptr, min_size);
  }
  if (ptr != NULL) free(ptr);
  return new_ptr;
}

void tl_free(void *ptr) { free(ptr); }
#endif
#endif
