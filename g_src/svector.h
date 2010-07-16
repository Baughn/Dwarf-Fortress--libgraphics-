//pelican aka sam dennis wrote this
#ifndef SVECTOR_H
#define SVECTOR_H

#include <vector>
#include <memory>

template <class T, class A = std::allocator<T> >
class svector : public std::vector<T, A> {
#ifdef __APPLE__
        public:
#endif

#ifndef WIN32
		using std::vector<T, A>::begin;
#endif

#ifndef __APPLE__
        public:
#endif
                void erase(typename std::vector<T, A>::size_type i) {
                        std::vector<T, A> &vec = *this;
                        vec.erase(begin() + i);
                }
                void insert(typename std::vector<T, A>::size_type i, const T &v) {

                        std::vector<T, A> &vec = *this;
                        vec.insert(begin() + i, v);
                }
};
#endif
