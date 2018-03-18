/*
This is intended to illustrate a problem I encountered in an
large and old codebase which is a combination of pure-C and C++
with the newer code being more C++, typically.

The challenge involves needing to use a custom allocator. This
allocator requries a constructor argument. Newer code is expected
to use the std:: containers, however it is not straight-forward
to use a custom allocator which lacks a no-arg constructor.

*/


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#include <limits>
#include <string>
#include <unordered_map>
#include <scoped_allocator>
#include <list>
using namespace std;

typedef unsigned int Tracking_memory_key;
class Host_user;

/* GLOBAL VARIABLES 1 */
Tracking_memory_key memory_key_a; /* this is not const */
Tracking_memory_key memory_key_b; /* this is not const */
Tracking_memory_key memory_key_c; /* this is not const */
Tracking_memory_key memory_key_d; /* this is not const */
Tracking_memory_key memory_key_e; /* this is not const */
Tracking_memory_key memory_key_f; /* this is not const */
Host_user **all_users= nullptr;
/* END GLOBAL VARIABLES 1 */



void load_mem_tracking_keys(void)
{
	memory_key_a= 0U;
	memory_key_b= 1U;
	memory_key_c= 1U;
	memory_key_d= 2U;
	memory_key_e= 3U;
	memory_key_f= 5U;
}

extern "C" void * tracking_malloc(Tracking_memory_key key, size_t size) {
	void *ptr= malloc(size + (sizeof(Tracking_memory_key)));
	fprintf(stderr, "tracking_malloc(key=%lu, size=%lu)=%p\n", (unsigned long) key, (unsigned long) size, ptr);
	*((Tracking_memory_key *)ptr) = key;
	return (void *)(((char *)ptr) + sizeof(Tracking_memory_key));
}

extern "C" void tracking_free(void *ptr)
{
	Tracking_memory_key key;
	char *pos;
	if (((unsigned long)ptr) > sizeof(Tracking_memory_key)) {
		pos = (((char *)ptr) - (sizeof(Tracking_memory_key)));
		key = *((Tracking_memory_key*)pos);
	} else {
		key = (Tracking_memory_key)UINT_MAX;
	}
	fprintf(stderr, "tracking_free(%p), (key: %lu)\n", ptr, (unsigned long)key);
	if (pos) {
		free(pos);
	}
}

/* Custom Allocator which requires a contructor argument */
#ifndef BOGUS_ALLOCATOR_INCLUDED
#define BOGUS_ALLOCATOR_INCLUDED
template <class T = void *> class Tracking_allocator
{
  // This cannot be const if we want to be able to swap.
  Tracking_memory_key m_key;

public:
  typedef T value_type;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

  typedef T* pointer;
  typedef const T* const_pointer;

  typedef T& reference;
  typedef const T& const_reference;

  pointer address(reference r) const { return &r; }
  const_pointer address(const_reference r) const { return &r; }

  explicit Tracking_allocator(Tracking_memory_key key) : m_key(key)
  {}

  template <class U> Tracking_allocator
    (const Tracking_allocator<U> &other __attribute__((unused)))
      : m_key(other.psi_key())
  {}

  template <class U> Tracking_allocator & operator=
    (const Tracking_allocator<U> &other __attribute__((unused)))
  {
    assert(m_key == other.psi_key()); // Don't swap key.
  }

  ~Tracking_allocator()
  {}

  pointer allocate(size_type n, const_pointer hint __attribute__((unused))= 0)
  {
    if (n == 0)
      return NULL;
    if (n > max_size())
      throw std::bad_alloc();

    size_t size= n * sizeof(T);
    pointer p= static_cast<pointer>(tracking_malloc(m_key, size));
    if (p == NULL)
      throw std::bad_alloc();

    return p;
  }

  void deallocate(pointer p, size_type) { tracking_free(p); }

  template <class U, class... Args>
  void construct(U *p, Args&&... args)
  {
    assert(p != NULL);
    try {
      ::new((void *)p) U(std::forward<Args>(args)...);
    } catch (...) {
      assert(false); // Constructor should not throw an exception.
    }
  }

  void destroy(pointer p)
  {
    assert(p != NULL);
    try {
      p->~T();
    } catch (...) {
      assert(false); // Destructor should not throw an exception
    }
  }

  size_type max_size() const
  {
    return std::numeric_limits<size_t>::max() / sizeof(T);
  }

  template <class U> struct rebind { typedef Tracking_allocator<U> other; };

  Tracking_memory_key psi_key() const { return m_key; }
};

template <class T>
bool operator== (const Tracking_allocator<T>& a1, const Tracking_allocator<T>& a2)
{
  return a1.psi_key() == a2.psi_key();
}

template <class T>
bool operator!= (const Tracking_allocator<T>& a1, const Tracking_allocator<T>& a2)
{
  return a1.psi_key() != a2.psi_key();
}

#endif // BOGUS_ALLOCATOR_INCLUDED

char *tracking_strdup(Tracking_memory_key key, const char *s)
{
	size_t len;
	char *dup;

	len = s ? strlen(s) : 0;
	dup = (char *)tracking_malloc(key, len + 1);
	if (!dup) {
		return NULL;
	}
	dup[len] = '\0';

	strcpy(dup, s);

	return dup;
}


#ifndef BOGUS_HOST_USER_INCLUDED
#define BOGUS_HOST_USER_INCLUDED
class Host_user {
      public:
	const char *id;
	const char *host;

	 Host_user(const char *id, const char *host) {
		this->id = tracking_strdup(memory_key_b, id ? id : "");
		this->host = tracking_strdup(memory_key_b, host ? host : "localhost");
	};
	~Host_user() {
		tracking_free((void *)this->id);
		tracking_free((void *)this->host);
	};
};
#endif // BOGUS_HOST_USER_INCLUDED


/* GLOBAL VARIABLES 2 */
/* This unordered_map should use Tracking_allocator with memory_key_d */
/* The list<Host_user *> should also allocate with memory_key_d */
typedef Tracking_allocator<Host_user *> Host_user_ptr_allocator;
typedef list < Host_user *, Host_user_ptr_allocator> Host_user_ptr_list;
typedef Tracking_allocator< pair<const string, Host_user_ptr_list> > name_hu_pair_allocator;
typedef scoped_allocator_adaptor< name_hu_pair_allocator, Host_user_ptr_allocator> Name_to_users_allocator;
typedef unordered_map < string, Host_user_ptr_list, hash<string>, equal_to<string>, Name_to_users_allocator > Name_to_users_map;

Name_to_users_map *name_to_users = nullptr;
/* END GLOBAL VARIABLES 2 */

void free_name_to_users(void)
{
	if (!name_to_users) return;

	name_to_users->~unordered_map();
	tracking_free(name_to_users);
	name_to_users = nullptr;
}


void clear_all_users()
{
	free_name_to_users();

	if (!all_users) return;

	for (size_t i = 0; all_users[i] != nullptr; ++i) {
		all_users[i]->~Host_user(); /* destroy */
		tracking_free(all_users[i]); /* free */
		all_users[i]= nullptr;
	}
	tracking_free(all_users);
	all_users = nullptr;
}

void build_name_to_users()
{
	fprintf(stderr, "build_name_to_users()\n");
	if (!all_users) return;
	if (name_to_users) {
		name_to_users->clear();
	} else {
		name_hu_pair_allocator outer(memory_key_d);
		Host_user_ptr_allocator inner(memory_key_d);
		Name_to_users_allocator adapter(outer, inner);

		size_t size= sizeof(Name_to_users_map);
		name_to_users= new(tracking_malloc(memory_key_d, size))
			Name_to_users_map(adapter);
	}
	for (size_t i = 0; all_users[i]; ++i) {
		Host_user *hu = all_users[i];
		string name = hu->id ? hu->id : "";
		(*name_to_users)[name].push_back(hu);
	}
	fprintf(stderr, "build_name_to_users() done\n");
}

void load_all_users()
{
	if (all_users) {
		clear_all_users();
	}
	size_t all_user_len = 15;
	size_t size = (all_user_len + 1) * sizeof(Host_user *);
	all_users = (Host_user **) tracking_malloc(memory_key_a, size);
	if (!all_users) {
		fprintf(stderr, "seriously? Couldn't allocate %lu bytes?\n",
			(unsigned long)size);
		exit(EXIT_FAILURE);
	}

	all_users[0] = new(tracking_malloc(memory_key_b, size)) Host_user("alice", "10.0.0.1");
	all_users[1] = new(tracking_malloc(memory_key_b, size)) Host_user("alice", "10.1.2.3");
	all_users[2] = new(tracking_malloc(memory_key_b, size)) Host_user("bob", "10.0.0.2");
	all_users[3] = new(tracking_malloc(memory_key_b, size)) Host_user("chris", "10.0.1.1");
	all_users[4] = new(tracking_malloc(memory_key_b, size)) Host_user("dirk", "10.0.1.2");
	all_users[5] = new(tracking_malloc(memory_key_b, size)) Host_user("eve", nullptr);
	all_users[6] = new(tracking_malloc(memory_key_b, size)) Host_user("eve", "10.0.0.2");
	all_users[7] = new(tracking_malloc(memory_key_b, size)) Host_user("eve", "10.0.1.2");
	all_users[8] = new(tracking_malloc(memory_key_b, size)) Host_user("eve", "10.0.2.2");
	all_users[9] = new(tracking_malloc(memory_key_b, size)) Host_user("eve", "10.0.3.2");
	all_users[10] = new(tracking_malloc(memory_key_b, size)) Host_user("eve", "10.8.0.2");
	all_users[11] = new(tracking_malloc(memory_key_b, size)) Host_user("frances", "10.0.1.2");
	all_users[12] = new(tracking_malloc(memory_key_b, size)) Host_user("glen", "10.8.0.7");
	all_users[13] = new(tracking_malloc(memory_key_b, size)) Host_user("harley", "10.8.0.7");
	all_users[14] = new(tracking_malloc(memory_key_b, size)) Host_user("ishta", "10.8.0.7");
	all_users[15] = NULL;

	build_name_to_users();
}

int print_name_to_users(FILE *stream)
{
	int rv; /* printf returns negative if error */
	int bytes_written = 0;
	for (auto it = (*name_to_users).begin(); it != (*name_to_users).end(); ++it) {
		string name = it->first;
		auto list = it->second;
		rv = fprintf(stream, "%s => {\n", name.c_str());
		if (rv < 0) goto err_print_name_to_users;
		bytes_written += rv;
		for (auto it2 = list.begin(); it2 != list.end(); ++it2) {
			Host_user *hu = *it2;
			rv = fprintf(stream, "\t`%s`@`%s`,\n", hu->id, hu->host);
			if (rv < 0) goto err_print_name_to_users;
			bytes_written += rv;
		}
		rv = fprintf(stream, "}\n");
		if (rv < 0) goto err_print_name_to_users;
		bytes_written += rv;
	}
	return bytes_written;

err_print_name_to_users:
	fprintf(stderr, "error writing to stream %p\n", (void *)stream);
	return rv;
}


int main(void)
{
	load_mem_tracking_keys();
	load_all_users();

	print_name_to_users(stdout);

	clear_all_users();

	return 0;
}
