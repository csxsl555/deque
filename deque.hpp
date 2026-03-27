#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

namespace sjtu {
    template <class T>
    class double_list {
    public:
        /**
         * elements
         * add whatever you want
         */

        int cur_size;
        struct Node {
            T *val;
            Node *pre, *nxt;
            Node() : val(nullptr), pre(nullptr), nxt(nullptr) {}
            Node(const T &v, Node *prev = nullptr, Node *next = nullptr) : val(new T(v)), pre(prev), nxt(next) {}
            ~Node() { delete val; }
        };
        Node *head, *tail;

        // --------------------------
        /**
         * the follows are constructors and destructors
         * you can also add some if needed.
         */
        double_list() : cur_size(0) {
            head = new Node();
            tail = new Node();
            head->nxt = tail;
            tail->pre = head;
        }

        double_list(const double_list<T> &other) : cur_size(0) {
            head = new Node();
            tail = new Node();
            head->nxt = tail;
            tail->pre = head;
            Node *cur = other.head->nxt;

            while (cur != other.tail) {
                insert_tail(*(cur->val));
                cur = cur->nxt;
            }
        }

        double_list<T> &operator=(const double_list<T> &other) {
            if (this == &other)
                return *this;
            clear();
            Node *cur = other.head->nxt;
            while (cur != other.tail) {
                insert_tail(*(cur->val));
                cur = cur->nxt;
            }
            return *this;
        }

        ~double_list() {
            Node *cur = head;
            while (cur != tail) {
                Node *tmp = cur->nxt;
                delete cur;
                cur = tmp;
            }
            delete cur;
        }

        class iterator {
        public:
            Node *iter;
            iterator() : iter(nullptr) {}
            iterator(Node *node) : iter(node) {}
            iterator(const iterator &t) : iter(t.iter) {}
            ~iterator() {}

            iterator operator++(int) {
                if (!iter || !iter->val)
                    throw invalid_iterator();
                iterator tmp = *this;
                iter = iter->nxt;
                return tmp;
            }
            iterator &operator++() {
                if (!iter || !iter->val)
                    throw invalid_iterator();
                iter = iter->nxt;
                return *this;
            }
            iterator operator--(int) {
                if (!iter || !iter->pre || !iter->pre->val)
                    throw invalid_iterator();
                iterator tmp = *this;
                iter = iter->pre;
                return tmp;
            }
            iterator &operator--() {
                if (!iter || !iter->pre || !iter->pre->val)
                    throw invalid_iterator();
                iter = iter->pre;
                return *this;
            }
            T &operator*() const {
                if (!iter || !iter->val)
                    throw "invalid";
                return *(iter->val);
            }
            T *operator->() const noexcept {
                return (iter && iter->val) ? iter->val : nullptr;
            }
            bool operator==(const iterator &rhs) const {
                return iter == rhs.iter;
            }
            bool operator!=(const iterator &rhs) const {
                return iter != rhs.iter;
            }
        };

        /**
         * return an iterator to the beginning
         */
        iterator begin() {
            return iterator(head->nxt);
        }
        iterator begin() const {
            return iterator(head->nxt);
        }

        /**
         * return an iterator to the ending
         */
        iterator end() {
            return iterator(tail);
        }
        iterator end() const {
            return iterator(tail);
        }

        /**
         * erase and return the iter to the same "index"
         */
        iterator erase(iterator pos) {
            if (!pos.iter || pos.iter == tail || pos.iter == head)
                return iterator(pos.iter);
            Node *cur = pos.iter;
            Node *nx = cur->nxt;
            cur->pre->nxt = cur->nxt;
            cur->nxt->pre = cur->pre;
            delete cur;
            cur_size--;
            return iterator(nx);
        }

        void insert_head(const T &val) {
            Node *cur = new Node(val, head, head->nxt);
            head->nxt->pre = cur;
            head->nxt = cur;
            cur_size++;
        }
        void insert_tail(const T &val) {
            Node *cur = new Node(val, tail->pre, tail);
            tail->pre->nxt = cur;
            tail->pre = cur;
            cur_size++;
        }
        void delete_head() {
            if (empty())
                return;
            Node *cur = head->nxt;
            head->nxt = cur->nxt;
            cur->nxt->pre = head;
            delete cur;
            cur_size--;
        }
        void delete_tail() {
            if (empty())
                return;
            Node *cur = tail->pre;
            cur->pre->nxt = tail;
            tail->pre = cur->pre;
            delete cur;
            cur_size--;
        }
        void remove_node(Node *node) {
            if (!node || node == head || node == tail)
                return;
            node->pre->nxt = node->nxt;
            node->nxt->pre = node->pre;
            node->pre = nullptr;
            node->nxt = nullptr;
            cur_size--;
        }
        void insert_existing(Node *node) {
            node->pre = head;
            node->nxt = head->nxt;
            head->nxt->pre = node;
            head->nxt = node;
            cur_size++;
        }
        void insert_existing_tail(Node *node) {
            node->nxt = tail;
            node->pre = tail->pre;
            tail->pre->nxt = node;
            tail->pre = node;
            cur_size++;
        }
        iterator erase_no_delete(iterator pos) {
            if (!pos.iter || pos.iter == tail || pos.iter == head)
                return iterator(pos.iter);
            Node *cur = pos.iter;
            Node *nx = cur->nxt;
            cur->pre->nxt = cur->nxt;
            cur->nxt->pre = cur->pre;
            cur->val = nullptr; // Don't delete the value
            delete cur;
            cur_size--;
            return iterator(nx);
        }
        bool empty() const {
            return head->nxt == tail;
        }
        bool empty() {
            return head->nxt == tail;
        }
        void clear() {
            while (!empty()) {
                delete_head();
            }
        }
        int size() const {
            return cur_size;
        }
    };

    template <class T>
    class deque {
    public:
        int total_size;
        static constexpr int BlockSize = 512;
        template <typename dataType>
        struct Node {
            dataType *data[BlockSize];
            int start;
            int end;

            Node() : start(BlockSize / 2), end(BlockSize / 2) {
                for (int i = 0; i < BlockSize; ++i)
                    data[i] = nullptr;
            }

            Node(const Node &other) : start(other.start), end(other.end) {
                for (int i = 0; i < BlockSize; ++i)
                    data[i] = nullptr;
                for (int i = start; i < end; ++i) {
                    if (other.data[i] != nullptr)
                        data[i] = new dataType(*other.data[i]);
                }
            }

            Node &operator=(const Node &other) {
                if (this == &other)
                    return *this;

                for (int i = start; i < end; ++i) {
                    delete data[i];
                    data[i] = nullptr;
                }

                start = other.start;
                end = other.end;

                for (int i = 0; i < BlockSize; ++i)
                    data[i] = nullptr;
                for (int i = start; i < end; ++i) {
                    if (other.data[i] != nullptr)
                        data[i] = new dataType(*other.data[i]);
                }
                return *this;
            }

            ~Node() {
                for (int i = start; i < end; ++i) {
                    delete data[i];
                }
            }

            int size() const { return end - start; }
            bool is_full() const { return end == BlockSize && start == 0; }
            bool is_empty() const { return start == end; }

            bool insert_tail(const dataType &value) {
                if (end == BlockSize)
                    return false;
                data[end] = new dataType(value);
                ++end;
                return true;
            }

            bool insert_head(const dataType &value) {
                if (start == 0)
                    return false;
                data[--start] = new dataType(value);
                return true;
            }

            void delete_tail() {
                --end;
                delete data[end];
                data[end] = nullptr;
            }

            void delete_head() {
                delete data[start];
                data[start] = nullptr;
                ++start;
            }

            void insert_backward(int pos, const dataType &value) {
                for (int i = start; i < pos; i += 1)
                    data[i - 1] = data[i];
                data[pos - 1] = new dataType(value);
                --start;
            }

            void insert_forward(int pos, const dataType &value) {
                for (int i = end - 1; i >= pos; i -= 1)
                    data[i + 1] = data[i];
                data[pos] = new dataType(value);
                ++end;
            }

            void delete_pos(int pos) {
                delete data[pos];
                if (pos - start < end - pos) {
                    for (int i = pos; i > start; --i)
                        data[i] = data[i - 1];
                    data[start] = nullptr;
                    ++start;
                } else {
                    for (int i = pos; i < end - 1; ++i)
                        data[i] = data[i + 1];
                    --end;
                    data[end] = nullptr;
                }
            }
        };
        double_list<Node<T>> array;

        using list_iterator = typename double_list<Node<T>>::iterator;
        using OuterNode = typename double_list<Node<T>>::Node;

        class const_iterator;
        class iterator {
            friend class deque<T>;
            friend class const_iterator;

        private:
            /**
             * add data members.
             * just add whatever you want.
             */
            deque<T> *owner;
            list_iterator block;
            int index;
            int rank;

        public:
            iterator() : owner(nullptr), block(), index(0), rank(0) {}

            iterator(deque<T> *o, const list_iterator &b, int i, int r)
                : owner(o), block(b), index(i), rank(r) {}

            iterator(const iterator &rhs)
                : owner(rhs.owner), block(rhs.block), index(rhs.index), rank(rhs.rank) {}

            iterator &operator=(const iterator &rhs) {
                if (this == &rhs)
                    return *this;
                owner = rhs.owner;
                block = rhs.block;
                index = rhs.index;
                rank = rhs.rank;
                return *this;
            }
            ~iterator() {}

            iterator jump_forward(const int &n) const {
                if (owner == nullptr)
                    throw invalid_iterator();
                if (block == owner->array.end())
                    throw invalid_iterator();
                OuterNode *cur_wrap = this->block.iter;
                OuterNode *nxt_wrap = cur_wrap->nxt;
                int cnt = n;
                if (index + cnt < cur_wrap->val->end)
                    return iterator(owner, block, index + cnt, rank + cnt);
                cnt -= block->end - index;
                cur_wrap = nxt_wrap, nxt_wrap = cur_wrap->nxt;
                if (cnt == 0) {
                    if (cur_wrap == owner->array.tail)
                        return owner->end();
                    else
                        return iterator(owner, cur_wrap, cur_wrap->val->start, rank + n);
                }
                while (cnt > 0) {
                    if (cur_wrap == owner->array.tail)
                        throw invalid_iterator();
                    if (cnt >= cur_wrap->val->end - cur_wrap->val->start) {
                        cnt -= cur_wrap->val->end - cur_wrap->val->start;
                        cur_wrap = nxt_wrap, nxt_wrap = cur_wrap->nxt;
                        if (cnt == 0) {
                            if (cur_wrap == owner->array.tail)
                                return owner->end();
                            else
                                return iterator(owner, cur_wrap, cur_wrap->val->start, rank + n);
                        }
                    } else {
                        int new_index = cur_wrap->val->start + cnt;
                        list_iterator new_block = cur_wrap;
                        return iterator(owner, new_block, new_index, rank + n);
                    }
                }
                throw invalid_iterator();
            }

            iterator jump_backward(const int &n) const {
                if (owner == nullptr)
                    throw invalid_iterator();
                OuterNode *cur_wrap;
                int pos;
                if (block == owner->array.end()) {
                    cur_wrap = owner->array.tail->pre;
                    if (cur_wrap == owner->array.head)
                        throw invalid_iterator();
                    pos = cur_wrap->val->end;
                } else {
                    cur_wrap = block.iter;
                    pos = index;
                }
                OuterNode *pre_wrap = cur_wrap->pre;
                int cnt = n;
                if (pos - cnt >= cur_wrap->val->start) {
                    list_iterator new_block = cur_wrap;
                    return iterator(owner, new_block, pos - cnt, rank - cnt);
                }
                cnt -= pos - cur_wrap->val->start + 1;
                cur_wrap = pre_wrap, pre_wrap = cur_wrap->pre;
                if (cnt == 0) {
                    if (cur_wrap == owner->array.head)
                        return owner->begin();
                    else
                        return iterator(owner, cur_wrap, cur_wrap->val->end - 1, rank - n);
                }
                while (cnt > 0) {
                    if (cur_wrap == owner->array.head)
                        throw invalid_iterator();
                    if (cnt >= cur_wrap->val->end - cur_wrap->val->start) {
                        cnt -= cur_wrap->val->end - cur_wrap->val->start;
                        cur_wrap = pre_wrap, pre_wrap = cur_wrap->pre;
                        if (cnt == 0) {
                            if (cur_wrap == owner->array.head)
                                return owner->begin();
                            else
                                return iterator(owner, cur_wrap, cur_wrap->val->end - 1, rank - n);
                        }
                    } else {
                        int new_index = cur_wrap->val->end - 1 - cnt;
                        list_iterator new_block = cur_wrap;
                        return iterator(owner, new_block, new_index, rank - n);
                    }
                }
                throw invalid_iterator();
            }

            /**
             * return a new iterator which points to the n-next element.
             * if there are not enough elements, the behaviour is undefined.
             * same for operator-.
             */
            iterator operator+(const int &n) const {
                if (n == 0)
                    return *this;
                else if (n > 0)
                    return jump_forward(n);
                else
                    return jump_backward(-n);
            }
            iterator operator-(const int &n) const {
                return *this + (-n);
            }

            /**
             * return the distance between two iterators.
             * if they point to different vectors, throw
             * invaild_iterator.
             */
            int operator-(const iterator &rhs) const {
                if (owner == nullptr || rhs.owner == nullptr || owner != rhs.owner)
                    throw invalid_iterator();
                return rank - rhs.rank;
            }
            iterator &operator+=(const int &n) {
                if (n == 0)
                    return *this;
                else if (n > 0)
                    return *this = jump_forward(n);
                else
                    return *this = jump_backward(-n);
            }
            iterator &operator-=(const int &n) {
                return (*this) += (-n);
            }

            /**
             * iter++
             */
            iterator operator++(int) {
                iterator tmp = *this;
                ++(*this);
                return tmp;
            }
            /**
             * ++iter
             */
            iterator &operator++() {
                if (owner == nullptr || block == owner->array.end())
                    throw invalid_iterator();

                ++rank;
                int blockEnd = block->end;
                if (index + 1 < blockEnd) {
                    ++index;
                    return *this;
                }

                ++block;
                if (block == owner->array.end()) {
                    index = 0;
                } else {
                    index = block->start;
                }
                return *this;
            }
            /**
             * iter--
             */
            iterator operator--(int) {
                iterator tmp = *this;
                --(*this);
                return tmp;
            }
            /**
             * --iter
             */
            iterator &operator--() {
                if (owner == nullptr)
                    throw invalid_iterator();

                --rank;
                if (block == owner->array.end()) {
                    if (owner->array.empty())
                        throw invalid_iterator();
                    --block;
                    index = block->end - 1;
                    return *this;
                }

                if (index > block->start) {
                    --index;
                    return *this;
                }

                if (block == owner->array.begin())
                    throw invalid_iterator();
                --block;
                index = block->end - 1;
                return *this;
            }

            /**
             * *it
             */
            T &operator*() const {
                if (owner == nullptr || block == owner->array.end())
                    throw invalid_iterator();
                if (index < block->start || index >= block->end)
                    throw invalid_iterator();
                if (block->data[index] == nullptr)
                    throw invalid_iterator();
                return *(block->data[index]);
            }
            /**
             * it->field
             */
            T *operator->() const noexcept {
                if (owner == nullptr || block == owner->array.end())
                    return nullptr;
                if (index < block->start || index >= block->end)
                    return nullptr;
                return block->data[index];
            }

            /**
             * check whether two iterators are the same (pointing to the same
             * memory).
             */
            bool operator==(const iterator &rhs) const {
                return owner == rhs.owner && block == rhs.block && index == rhs.index;
            }
            bool operator==(const const_iterator &rhs) const {
                return owner == rhs.owner && block == rhs.block && index == rhs.index;
            }
            /**
             * some other operator for iterators.
             */
            bool operator!=(const iterator &rhs) const {
                return !(*this == rhs);
            }
            bool operator!=(const const_iterator &rhs) const {
                return !(*this == rhs);
            }
        };

        class const_iterator {
            /**
             * it should has similar member method as iterator.
             * you can copy them, but with care!
             * and it should be able to be constructed from an iterator.
             */
            friend class deque<T>;
            friend class iterator;

        private:
            /**
             * add data members.
             * just add whatever you want.
             */
            const deque<T> *owner;
            list_iterator block;
            int index;
            int rank;

        public:
            const_iterator() : owner(nullptr), block(), index(0), rank(0) {}

            const_iterator(const deque<T> *o, const list_iterator &b, int i, int r)
                : owner(o), block(b), index(i), rank(r) {}

            const_iterator(const const_iterator &rhs)
                : owner(rhs.owner), block(rhs.block), index(rhs.index), rank(rhs.rank) {}

            const_iterator(const iterator &rhs)
                : owner(rhs.owner), block(rhs.block), index(rhs.index), rank(rhs.rank) {}

            const_iterator &operator=(const iterator &rhs) {
                if (this == &rhs)
                    return *this;
                owner = rhs.owner;
                block = rhs.block;
                index = rhs.index;
                rank = rhs.rank;
                return *this;
            }
            ~const_iterator() {}

            const_iterator jump_forward(const int &n) const {
                if (owner == nullptr)
                    throw invalid_iterator();
                if (block == owner->array.end())
                    throw invalid_iterator();
                OuterNode *cur_wrap = this->block.iter;
                OuterNode *nxt_wrap = cur_wrap->nxt;
                int cnt = n;
                if (index + cnt < cur_wrap->val->end)
                    return const_iterator(owner, block, index + cnt, rank + cnt);
                cnt -= block->end - index;
                cur_wrap = nxt_wrap, nxt_wrap = cur_wrap->nxt;
                if (cnt == 0) {
                    if (cur_wrap == owner->array.tail)
                        return owner->cend();
                    else
                        return const_iterator(owner, cur_wrap, cur_wrap->val->start, rank + n);
                }
                while (cnt > 0) {
                    if (cur_wrap == owner->array.tail)
                        throw invalid_iterator();
                    if (cnt >= cur_wrap->val->end - cur_wrap->val->start) {
                        cnt -= cur_wrap->val->end - cur_wrap->val->start;
                        cur_wrap = nxt_wrap, nxt_wrap = cur_wrap->nxt;
                        if (cnt == 0) {
                            if (cur_wrap == owner->array.tail)
                                return owner->cend();
                            else
                                return const_iterator(owner, cur_wrap, cur_wrap->val->start, rank + n);
                        }
                    } else {
                        int new_index = cur_wrap->val->start + cnt;
                        list_iterator new_block = cur_wrap;
                        return const_iterator(owner, new_block, new_index, rank + n);
                    }
                }
                throw invalid_iterator();
            }

            const_iterator jump_backward(const int &n) const {
                if (owner == nullptr)
                    throw invalid_iterator();
                OuterNode *cur_wrap;
                int pos;
                if (block == owner->array.end()) {
                    cur_wrap = owner->array.tail->pre;
                    if (cur_wrap == owner->array.head)
                        throw invalid_iterator();
                    pos = cur_wrap->val->end;
                } else {
                    cur_wrap = block.iter;
                    pos = index;
                }
                OuterNode *pre_wrap = cur_wrap->pre;
                int cnt = n;
                if (pos - cnt >= cur_wrap->val->start) {
                    list_iterator new_block = cur_wrap;
                    return const_iterator(owner, new_block, pos - cnt, rank - cnt);
                }
                cnt -= pos - cur_wrap->val->start + 1;
                cur_wrap = pre_wrap, pre_wrap = cur_wrap->pre;
                if (cnt == 0) {
                    if (cur_wrap == owner->array.head)
                        return owner->cbegin();
                    else
                        return const_iterator(owner, cur_wrap, cur_wrap->val->end, rank - n);
                }
                while (cnt > 0) {
                    if (cur_wrap == owner->array.head)
                        throw invalid_iterator();
                    if (cnt >= cur_wrap->val->end - cur_wrap->val->start) {
                        cnt -= cur_wrap->val->end - cur_wrap->val->start;
                        cur_wrap = pre_wrap, pre_wrap = cur_wrap->pre;
                        if (cnt == 0) {
                            if (cur_wrap == owner->array.head)
                                return owner->cbegin();
                            else
                                return const_iterator(owner, cur_wrap, cur_wrap->val->end - 1, rank - n);
                        }
                    } else {
                        int new_index = cur_wrap->val->end - 1 - cnt;
                        list_iterator new_block = cur_wrap;
                        return const_iterator(owner, new_block, new_index, rank - n);
                    }
                }
                throw invalid_iterator();
            }
            /**
             * return a new iterator which points to the n-next element.
             * if there are not enough elements, the behaviour is undefined.
             * same for operator-.
             */
            const_iterator operator+(const int &n) const {
                if (n == 0)
                    return *this;
                else if (n > 0)
                    return jump_forward(n);
                else
                    return jump_backward(-n);
            }
            const_iterator operator-(const int &n) const {
                return *this + (-n);
            }

            /**
             * return the distance between two iterators.
             * if they point to different vectors, throw
             * invaild_iterator.
             */
            int operator-(const const_iterator &rhs) const {
                if (owner == nullptr || rhs.owner == nullptr || owner != rhs.owner)
                    throw invalid_iterator();
                return rank - rhs.rank;
            }
            const_iterator &operator+=(const int &n) {
                if (n == 0)
                    return *this;
                else if (n > 0)
                    return *this = jump_forward(n);
                else
                    return *this = jump_backward(-n);
            }
            const_iterator &operator-=(const int &n) {
                return (*this) += (-n);
            }

            /**
             * iter++
             */
            const_iterator operator++(int) {
                const_iterator tmp = *this;
                ++(*this);
                return tmp;
            }
            /**
             * ++iter
             */
            const_iterator &operator++() {
                if (owner == nullptr || block == owner->array.end())
                    throw invalid_iterator();

                ++rank;
                int blockEnd = block->end;
                if (index + 1 < blockEnd) {
                    ++index;
                    return *this;
                }

                ++block;
                if (block == owner->array.end()) {
                    index = 0;
                } else {
                    index = block->start;
                }
                return *this;
            }
            /**
             * iter--
             */
            const_iterator operator--(int) {
                iterator tmp = *this;
                --(*this);
                return tmp;
            }
            /**
             * --iter
             */
            const_iterator &operator--() {
                if (owner == nullptr)
                    throw invalid_iterator();

                --rank;
                if (block == owner->array.end()) {
                    if (owner->array.empty())
                        throw invalid_iterator();
                    --block;
                    index = block->end - 1;
                    return *this;
                }

                if (index > block->start) {
                    --index;
                    return *this;
                }

                if (block == owner->array.begin())
                    throw invalid_iterator();
                --block;
                index = block->end - 1;
                return *this;
            }

            /**
             * *it
             */
            const T &operator*() const {
                if (owner == nullptr || block == owner->array.end())
                    throw invalid_iterator();
                if (index < block->start || index >= block->end)
                    throw invalid_iterator();
                if (block->data[index] == nullptr)
                    throw invalid_iterator();
                return *(block->data[index]);
            }
            /**
             * it->field
             */
            const T *operator->() const noexcept {
                if (owner == nullptr || block == owner->array.end())
                    return nullptr;
                if (index < block->start || index >= block->end)
                    return nullptr;
                return block->data[index];
            }

            /**
             * check whether two iterators are the same (pointing to the same
             * memory).
             */
            bool operator==(const iterator &rhs) const {
                return owner == rhs.owner && block == rhs.block && index == rhs.index;
            }
            bool operator==(const const_iterator &rhs) const {
                return owner == rhs.owner && block == rhs.block && index == rhs.index;
            }
            /**
             * some other operator for iterators.
             */
            bool operator!=(const iterator &rhs) const {
                return !(*this == rhs);
            }
            bool operator!=(const const_iterator &rhs) const {
                return !(*this == rhs);
            }
        };

        /**
         * constructors.
         */
        deque() : total_size(0) {}
        deque(const deque &other) {
            array.clear();
            array = other.array;
            total_size = other.total_size;
        }

        /**
         * deconstructor.
         */
        ~deque() {
            array.clear();
        }

        /**
         * assignment operator.
         */
        deque &operator=(const deque &other) {
            if (this == &other)
                return *this;
            array = other.array;
            total_size = other.total_size;
            return *this;
        }

        /**
         * access a specified element with bound checking.
         * throw index_out_of_bound if out of bound.
         */
        T &at(const size_t &pos) {
            if (pos >= total_size)
                throw index_out_of_bound();
            return *(this->begin() + pos);
        }
        const T &at(const size_t &pos) const {
            if (pos >= total_size)
                throw index_out_of_bound();
            return *(this->cbegin() + pos);
        }
        T &operator[](const size_t &pos) {
            if (pos >= total_size)
                throw index_out_of_bound();
            return this->at(pos);
        }
        const T &operator[](const size_t &pos) const {
            if (pos >= total_size)
                throw index_out_of_bound();
            return this->at(pos);
        }

        /**
         * access the first element.
         * throw container_is_empty when the container is empty.
         */
        const T &front() const {
            if (array.empty())
                throw container_is_empty();
            return *(this->cbegin());
        }
        /**
         * access the last element.
         * throw container_is_empty when the container is empty.
         */
        const T &back() const {
            if (array.empty())
                throw container_is_empty();
            return *(--(this->cend()));
        }

        /**
         * return an iterator to the beginning.
         */
        iterator begin() {
            if (array.empty())
                return end();
            list_iterator b = array.begin();
            return iterator(this, b, (*b).start, 0);
        }
        const_iterator cbegin() const {
            if (array.empty())
                return cend();
            list_iterator b = array.begin();
            return const_iterator(this, b, (*b).start, 0);
        }

        /**
         * return an iterator to the end.
         */
        iterator end() {
            return iterator(this, array.end(), 0, total_size);
        }
        const_iterator cend() const {
            return const_iterator(this, array.end(), 0, total_size);
        }

        /**
         * check whether the container is empty.
         */
        bool empty() const { return array.empty(); }

        /**
         * return the number of elements.
         */
        size_t size() const { return total_size; }

        /**
         * clear all contents.
         */
        void clear() {
            array.clear();
            total_size = 0;
        }

        /**
         * insert value before pos.
         * return an iterator pointing to the inserted value.
         * throw if the iterator is invalid or it points to a wrong place.
         */
        iterator insert(iterator pos, const T &value) {
            if (pos.owner != this)
                throw invalid_iterator();

            if (pos.block == array.end()) {
                if (pos.index != 0)
                    throw invalid_iterator();
                push_back(value);
                iterator ret = end();
                --ret;
                return ret;
            }

            if (pos.index < pos.block->start || pos.index > pos.block->end)
                throw invalid_iterator();
            ++total_size;
            int beg = pos.block->start, now_pos = pos.index, end = pos.block->end, ins_rank = pos.rank;
            if (beg > 0) {
                pos.block->insert_backward(now_pos, value);
                --pos.index;
                return pos;
            } else if (end < BlockSize) {
                pos.block->insert_forward(now_pos, value);
                return pos;
            } else {
                Node<T> new_node;
                for (int i = BlockSize / 2; i < BlockSize; i += 1) {
                    new_node.insert_tail(*(pos.block->data[i]));
                }
                for (int i = BlockSize / 2; i < BlockSize; i += 1) {
                    pos.block->delete_tail();
                }
                OuterNode *cur_wrap = pos.block.iter;
                OuterNode *nxt_wrap = cur_wrap->nxt;
                OuterNode *new_wrap = new OuterNode(new_node, cur_wrap, nxt_wrap);
                cur_wrap->nxt = new_wrap;
                nxt_wrap->pre = new_wrap;
                ++array.cur_size;

                if (now_pos < BlockSize / 2) {
                    if (pos.block->start > 0) {
                        pos.block->insert_backward(now_pos, value);
                        --pos.index;
                        return pos;
                    } else {
                        pos.block->insert_forward(now_pos, value);
                        return pos;
                    }
                } else {
                    list_iterator new_block = new_wrap;
                    int new_pos = new_block->start + (now_pos - BlockSize / 2);

                    if (new_block->start > 0) {
                        new_block->insert_backward(new_pos, value);
                        return iterator(this, new_block, new_pos - 1, ins_rank);
                    } else {
                        new_block->insert_forward(new_pos, value);
                        return iterator(this, new_block, new_pos, ins_rank);
                    }
                }
            }
        }

        /**
         * remove the element at pos.
         * return an iterator pointing to the following element. if pos points to
         * the last element, return end(). throw if the container is empty,
         * the iterator is invalid, or it points to a wrong place.
         */
        iterator erase(iterator pos) {
            if (empty())
                throw container_is_empty();
            if (pos.owner != this || pos.block == array.end())
                throw invalid_iterator();
            if (pos.index < pos.block->start || pos.index >= pos.block->end)
                throw invalid_iterator();

            list_iterator old_block = pos.block;
            int old_index = pos.index;
            int old_rank = pos.rank;
            int old_start = old_block->start;
            int old_end = old_block->end;
            bool shift_from_left = (old_index - old_start < old_end - old_index);

            pos.block->delete_pos(pos.index);
            --total_size;

            if (pos.block->is_empty()) {
                OuterNode *cur_wrap = pos.block.iter;
                OuterNode *nxt_wrap = cur_wrap->nxt;

                array.remove_node(cur_wrap);
                delete cur_wrap;

                if (nxt_wrap == array.tail)
                    return end();
                list_iterator nxt_block = nxt_wrap;
                return iterator(this, nxt_block, nxt_block->start, old_rank);
            } else {
                int candidate_index = old_index + (shift_from_left ? 1 : 0);
                if (candidate_index < old_block->end)
                    return iterator(this, old_block, candidate_index, old_rank);
                else {
                    OuterNode *cur_wrap = pos.block.iter;
                    OuterNode *nxt_wrap = cur_wrap->nxt;
                    if (nxt_wrap == array.tail)
                        return end();
                    list_iterator nxt_block = nxt_wrap;
                    return iterator(this, nxt_block, nxt_block->start, old_rank);
                }
            }
        }

        /**
         * add an element to the end.
         */
        void push_back(const T &value) {
            if (array.empty())
                array.insert_tail(Node<T>());
            list_iterator it = array.end();
            --it;
            if (!(*it).insert_tail(value)) {
                array.insert_tail(Node<T>());
                it = array.end();
                --it;
                (*it).insert_tail(value);
            }
            ++total_size;
        }

        /**
         * remove the last element.
         * throw when the container is empty.
         */
        void pop_back() {
            if (total_size == 0)
                throw container_is_empty();
            list_iterator it = array.end();
            --it;
            (*it).delete_tail();
            if ((*it).is_empty())
                array.delete_tail();
            --total_size;
        }

        /**
         * insert an element to the beginning.
         */
        void push_front(const T &value) {
            if (array.empty())
                array.insert_head(Node<T>());
            list_iterator it = array.begin();
            if (!(*it).insert_head(value)) {
                array.insert_head(Node<T>());
                it = array.begin();
                (*it).insert_head(value);
            }
            ++total_size;
        }

        /**
         * remove the first element.
         * throw when the container is empty.
         */
        void pop_front() {
            if (total_size == 0)
                throw container_is_empty();
            list_iterator it = array.begin();
            (*it).delete_head();
            if ((*it).is_empty())
                array.delete_head();
            --total_size;
        }
    };

} // namespace sjtu

#endif
