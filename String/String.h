/*********************************************************
Developer: MilesYang
Mail: 1961464399@qq.com
FileName: String.h
*********************************************************/

#ifndef _YL_STRING_H_
#define _YL_STRING_H_

#include <exception>
#include <stdexcept>
#include <memory>
#include <cstring>
#include <type_traits>
#include <iterator>
#include <iostream>

#pragma warning(disable:4290)

#define DECLARE_ERROR(error_name, base_class_name) \
	class error_name :public base_class_name\
	{\
	public:\
		explicit error_name(char const* const msg)noexcept :base_class_name(msg == nullptr ? #error_name : msg) {}\
		error_name()noexcept :StringError(#error_name) {}\
	}
#define DECLARE_STRING_ERROR(name) DECLARE_ERROR(name, StringError)
#define nonconst
#define THROW_STD_EXCEPT throw(std::exception)

#ifdef _MSC_VER
#define INLINE __forceinline
#else
#define INLINE __attribute__((always_inline)) inline
#endif // _MSC_VER

namespace cildhdi
{
	namespace errors
	{

		class StringError :public std::exception
		{
		private:
			const char* const _what_error;
		public:
			explicit StringError(const char* msg)noexcept : _what_error(msg) {};
			StringError()noexcept : _what_error("StringError") {}
		};
		DECLARE_STRING_ERROR(OutOfRange);
		DECLARE_STRING_ERROR(NullPtr);
		DECLARE_STRING_ERROR(NotCharOrWChar);
		DECLARE_STRING_ERROR(DifferentObj);
		DECLARE_STRING_ERROR(UnsupportedType);

	} //namespace errors

	namespace helpers
	{

		template<typename E>
		INLINE void expect(bool condition) throw(E)
		{
			if (!condition)
			{
				throw E();
			}
		}

		template<typename Char>
		INLINE void* copy_str(Char* dst, const Char* src, size_t size) THROW_STD_EXCEPT
		{
			helpers::expect<errors::NullPtr>(dst != nullptr);
			helpers::expect<errors::NullPtr>(src != nullptr);
			if (size == 0) return nullptr;
			return std::memcpy(dst, src, size * sizeof(Char));
		}

		template<typename Char>
		INLINE std::shared_ptr<Char> make_chars_shared_ptr(Char* ptr)
		{
			return std::shared_ptr<Char>(ptr, std::default_delete<Char[]>());
		}

		template<typename Char>
		INLINE std::shared_ptr<Char> get_copy_str(const Char* src, size_t size, bool with_zero = false) THROW_STD_EXCEPT
		{
			if (size == 0 && !with_zero) return nullptr;
			Char* res = new Char[size + static_cast<size_t>(with_zero ? 1 : 0)];
			std::memset(res, 0, (size + static_cast<size_t>(with_zero ? 1 : 0)) * sizeof(Char));
			if (src != nullptr) copy_str(res, src, size);
			return make_chars_shared_ptr(res);
		}

		template<typename Char>
		INLINE size_t strlen(const Char* str)
		{
			return 0;
		}

		template<>
		INLINE size_t strlen<char>(const char* str)
		{
			return std::strlen(str);
		}

		template<>
		INLINE size_t strlen<wchar_t>(const wchar_t* str)
		{
			return std::wcslen(str);
		}

		template<typename Char>
		INLINE Char* strstr(Char* str, Char* target)
		{
			return nullptr;
		}

		template<>
		INLINE char* strstr<char>(char* str, char* target)
		{
			return std::strstr(str, target);
		}

		template<>
		INLINE wchar_t* strstr<wchar_t>(wchar_t* str, wchar_t* target)
		{
			return std::wcsstr(str, target);
		}

		template<typename Char>
		class part_specialization_vtos
		{
		public:
			template<typename V>
			static int vtos(Char* buffer, size_t size, const Char* format, V& value)
			{
				helpers::expect<errors::UnsupportedType>(false);
				return 0;
			}
		};

		template<>
		class part_specialization_vtos<char>
		{
		public:
			template<typename V>
			static int vtos(char* buffer, size_t size, const char* format, V& value)
			{
				return std::snprintf(buffer, size, format, value);
			}
		};

		template<>
		class part_specialization_vtos<wchar_t>
		{
		public:
			template<typename V>
			static int vtos(wchar_t* buffer, size_t size, const wchar_t* format, V& value)
			{
				return std::swprintf(buffer, size, format, value);
			}
		};


		template<typename V>
		class part_specialization_fmt
		{
		public:
			template<typename Char>
			static Char* get_format(Char* format)
			{
				helpers::expect<errors::UnsupportedType>(false);
				return format;
			}
		};

		template<>
		class part_specialization_fmt<char>
		{
		public:
			template<typename Char>
			static Char* get_format(Char* format)
			{
				format[0] = '%'; format[1] = 'c';
				return format;
			}
		};

		template<>
		class part_specialization_fmt<int>
		{
		public:
			template<typename Char>
			static Char* get_format(Char* format)
			{
				format[0] = '%'; format[1] = 'd';
				return format;
			}
		};

		template<>
		class part_specialization_fmt<long>
		{
		public:
			template<typename Char>
			static Char* get_format(Char* format)
			{
				format[0] = '%'; format[1] = 'l'; format[2] = 'd';
				return format;
			}
		};

		template<>
		class part_specialization_fmt<long long>
		{
		public:
			template<typename Char>
			static Char* get_format(Char* format)
			{
				format[0] = '%'; format[1] = 'l'; format[2] = 'l'; format[3] = 'd';
				return format;
			}
		};

		template<>
		class part_specialization_fmt<unsigned int>
		{
		public:
			template<typename Char>
			static Char* get_format(Char* format)
			{
				format[0] = '%'; format[1] = 'u';
				return format;
			}
		};

		template<>
		class part_specialization_fmt< unsigned long>
		{
		public:
			template<typename Char>
			static Char* get_format(Char* format)
			{
				format[0] = '%'; format[1] = 'l'; format[2] = 'u';
				return format;
			}
		};

		template<>
		class part_specialization_fmt<unsigned long long>
		{
		public:
			template<typename Char>
			static Char* get_format(Char* format)
			{
				format[0] = '%'; format[1] = 'l'; format[2] = 'l'; format[3] = 'u';
				return format;
			}
		};

		template<>
		class part_specialization_fmt<double>
		{
		public:
			template<typename Char>
			static Char* get_format(Char* format)
			{
				format[0] = '%'; format[1] = 'l'; format[2] = 'f';
				return format;
			}
		};


		template<>
		class part_specialization_fmt<long double>
		{
		public:
			template<typename Char>
			static Char* get_format(Char* format)
			{
				format[0] = '%'; format[1] = 'L'; format[2] = 'f';
				return format;
			}
		};

	} //helpers

	template<typename Char> class StringBase;

	namespace iterators
	{

		template<typename Char>
		class IteratorBase : public std::iterator<std::random_access_iterator_tag, Char>
		{
		public:
			using Object = StringBase<Char>;
			using ObjectPtr = StringBase<Char>*;
			using IteratorReference = IteratorBase & ;
			using CharType = Char;
			using CharReference = Char & ;
			using CharPtrType = Char * ;
			friend class StringBase<Char>;
		public:
			IteratorBase() :_obj(nullptr), _position(nullptr)
			{
			}
		public: //operators
			IteratorReference operator+=(typename IteratorBase::difference_type n) THROW_STD_EXCEPT
			{
				_range_check(n);
				_position += n;
				return *this;
			}

			IteratorBase operator+(typename IteratorBase::difference_type n)const THROW_STD_EXCEPT
			{
				_range_check(n);
				return IteratorBase(_obj, _position + n);
			}

			IteratorReference operator-=(typename IteratorBase::difference_type n) THROW_STD_EXCEPT
			{
				_range_check(-n);
				_position -= n;
				return *this;
			}

			IteratorBase operator-(typename IteratorBase::difference_type n)const THROW_STD_EXCEPT
			{
				_range_check(-n);
				return IteratorBase(_obj, _position - n);
			}

			typename IteratorBase<Char>::difference_type operator-(const IteratorBase<Char>& rhs)const THROW_STD_EXCEPT
			{
				if (_position == nullptr && rhs._position == nullptr)
				{
					return 0;
				}
				else
				{
					_nullptr_check();
					rhs._nullptr_check();
					_dif_obj_check(rhs);
				}
				return _position - rhs._position;
			}

			IteratorBase operator[](typename IteratorBase::difference_type n)
			{
				_range_check(n);
				return *this + n;
			}

			bool operator<(const IteratorBase& rhs)const THROW_STD_EXCEPT
			{
				_dif_obj_check(rhs);
				return _position < rhs._position;
			}

			bool operator>(const IteratorBase& rhs)const THROW_STD_EXCEPT
			{
				_dif_obj_check(rhs);
				return _position > rhs._position;
			}

			bool operator<=(const IteratorBase& rhs)const THROW_STD_EXCEPT
			{
				_dif_obj_check(rhs);
				return _position <= rhs._position;
			}

			bool operator>=(const IteratorBase& rhs)const THROW_STD_EXCEPT
			{
				_dif_obj_check(rhs);
				return _position >= rhs._position;
			}

			IteratorReference operator--()
			{
				_nullptr_check();
				_range_check(-1);
				--_position;
				return *this;
			}

			IteratorBase operator--(int) const
			{
				_nullptr_check();
				_range_check(-1);
				return IteratorBase(_obj, _position - 1);
			}

			IteratorReference operator++()
			{
				_nullptr_check();
				_range_check(1);
				++_position;
				return *this;
			}

			IteratorBase operator++(int) const
			{
				_nullptr_check();
				_range_check(1);
				return IteratorBase(_obj, _position + 1);
			}

			CharReference operator*() THROW_STD_EXCEPT
			{
				_nullptr_check();
				return *_position;
			}

			CharPtrType operator->()
			{
				_nullptr_check();
				return _position;
			}

			bool operator==(const IteratorBase& rhs) const
			{
				return (_obj != nullptr&&rhs._obj != nullptr&&_obj == rhs._obj&&_position == rhs._position);
			}

			bool operator!=(const IteratorBase& rhs) const
			{
				return !(operator==(rhs));
			}
		private:
			IteratorBase(ObjectPtr obj_ptr, CharPtrType char_ptr) :_obj(obj_ptr), _position(char_ptr)
			{

			}

			INLINE void _range_check(typename IteratorBase::difference_type d)const THROW_STD_EXCEPT
			{
				_nullptr_check();
				typename IteratorBase::difference_type rd = std::distance(_obj->_data.get(), _position + d);
				helpers::expect<errors::OutOfRange>(rd >= 0 && rd <= static_cast<typename IteratorBase::difference_type>(_obj->size()));
			}

			INLINE void _dif_obj_check(const IteratorBase& rhs)const THROW_STD_EXCEPT
			{
				_nullptr_check();
				rhs._nullptr_check();
				helpers::expect<errors::DifferentObj>(_obj == rhs._obj);
			}

			INLINE void _nullptr_check() const THROW_STD_EXCEPT
			{
				helpers::expect<errors::NullPtr>(_obj != nullptr && _position != nullptr);
			}
		private:
			ObjectPtr _obj;
			CharPtrType _position;
		};

		template<typename Char>
		IteratorBase<Char> operator+(typename IteratorBase<Char>::difference_type n, IteratorBase<Char> it) THROW_STD_EXCEPT
		{
			return it + n;
		}


	} //namespace iterators

	template<typename Char>
	class StringBase
	{
	public: //Types declare
		using CharType = Char;
		using CharReference = Char & ;
		using CharPtrType = Char * ;
		using ConstCharPtr = const Char*;
		using CharSharedPtrType = std::shared_ptr<Char>;
		using Iterator = iterators::IteratorBase<CharType>;
		friend class iterators::IteratorBase<CharType>;
	public: //Constructors
		StringBase() THROW_STD_EXCEPT
		{
			reset();
		}

		template<typename V>
		StringBase(V value, typename std::enable_if<std::is_arithmetic<V>::value>::type* = 0) THROW_STD_EXCEPT
		{
			reset();
			CharType format[5] = { 0 };
			CharType buffer[50] = { 0 };
			helpers::part_specialization_vtos<CharType>::vtos(buffer, 50, helpers::part_specialization_fmt<V>::get_format(format), value);
			_size = helpers::strlen(buffer);
			if (_size == 0)
				return;
			_capacity = _size;
			_data = helpers::get_copy_str<CharType>(buffer, _size, false);
		}

		StringBase(const StringBase& other) THROW_STD_EXCEPT
		{
			reset();
			_data = other._data;
			_size = other._size;
			_capacity = other._capacity;
			_cur_arg_index = other._cur_arg_index;
		}

		StringBase(StringBase&& other) THROW_STD_EXCEPT
		{
			reset();
			swap(other);
		}

		StringBase(const CharType* str, size_t count) THROW_STD_EXCEPT
		{
			reset();
			_size = (count == npos ? helpers::strlen(str) : count);
			if (_size == 0)
			{
				return;
			}
			_capacity = _size;
			_data = helpers::get_copy_str<CharType>(str, _size, false);
		}

		StringBase(const CharType* str) THROW_STD_EXCEPT
		{
			reset();
			_size = helpers::strlen(str);
			if (_size == 0)
			{
				return;
			}
			_capacity = _size;
			_data = helpers::get_copy_str<CharType>(str, _size, false);
		}
	public: //Element access
		CharType at(size_t index) const THROW_STD_EXCEPT
		{
			_range_check(index);
			CharType c = *(_data.get() + index);
			return c;
		}

		CharReference at(size_t index) nonconst THROW_STD_EXCEPT
		{
			_call_non_const_func();
			_range_check(index);
			return *(_data.get() + index);
		}

		CharReference front() nonconst THROW_STD_EXCEPT
		{
			_call_non_const_func();
			_range_check(0);//UB
			return *(_data.get());
		}

		CharReference back() nonconst THROW_STD_EXCEPT
		{
			_call_non_const_func();
			_range_check(_size - 1);
			return *(_data.get() + _size - 1);
		}

		CharSharedPtrType c_str() const THROW_STD_EXCEPT
		{
			return helpers::get_copy_str(_data.get(), _size, true);
		}

	public: //Iterators
		Iterator begin() nonconst THROW_STD_EXCEPT
		{
			_call_non_const_func();
			return Iterator(this, _data.get());
		}

		Iterator end() nonconst THROW_STD_EXCEPT
		{
			_call_non_const_func();
			if (_size == 0)
				return begin();
			else
				return Iterator(this, (static_cast<CharPtrType>(_data.get()) + _size));
		}

	public: //Capacity
		size_t size() const
		{
			return _size;
		}

		size_t capacity() const
		{
			return _capacity;
		}

		bool empty() const
		{
			return _size == 0;
		}

		void resize(size_t size, CharType ch = CharType()) nonconst THROW_STD_EXCEPT
		{
			if (size < _size)
			{
				_size = size;
				_capacity = _size;
				_call_non_const_func();
			}
			else if (size > _size)
			{
				_data = helpers::get_copy_str(_data.get(), size, false);
				for (size_t i = _size; i < size; i++)
				{
					*(_data.get() + i) = ch;
				}
				_size = size;
				_capacity = _size;
			}
			else
			{
				return;
			}
		}

		void reserve(size_t size) nonconst THROW_STD_EXCEPT
		{
			_call_non_const_func();
			if (size > _capacity)
			{
				_data = helpers::get_copy_str(_data.get(), size, false);
				_capacity = size;
			}
		}

	public: //Operations
		void reset() nonconst THROW_STD_EXCEPT
		{
			_data.~shared_ptr();
			_data = helpers::make_chars_shared_ptr<CharType>(nullptr);
			_size = 0;
			_capacity = 0;
			_cur_arg_index = 1;
		}

		void clear() nonconst THROW_STD_EXCEPT
		{
			_call_non_const_func();
			_size = 0;
		}

		StringBase& append(CharType ch, size_t count = 1) nonconst THROW_STD_EXCEPT
		{
			if (count == 0) return *this;
			_call_non_const_func();
			size_t old = size();
			reserve(_get_expand_size(count));
			_size += count;
			for (size_t i = old; i < _size; i++)
			{
				at(i) = ch;
			}
			return *this;
		}

		StringBase& append(const StringBase& str) nonconst THROW_STD_EXCEPT
		{
			if (str.empty()) return *this;
			_call_non_const_func();
			size_t old = size();
			reserve(_get_expand_size(str.size()));
			_size += str.size();
			helpers::copy_str(_data.get() + old, str._data.get(), str.size());
			return *this;
		}

		StringBase& append(const StringBase& str, size_t pos, size_t count) nonconst THROW_STD_EXCEPT
		{
			if (str.empty() || count == 0) return *this;
			_call_non_const_func();
			helpers::expect<errors::OutOfRange>(pos < str.size());
			size_t old = size();
			if (count == npos) count = str.size() - pos;
			reserve(_get_expand_size(count));
			_size += count;
			helpers::copy_str(_data.get() + old, str._data.get() + pos, count);
			return *this;
		}

		void push_back(CharType ch) nonconst THROW_STD_EXCEPT
		{
			_call_non_const_func();
			append(ch);
		}

		void pop_back() nonconst THROW_STD_EXCEPT
		{
			_call_non_const_func();
			helpers::expect<errors::OutOfRange>(_size != 0);
			--_size;
		}

		int compare(const StringBase& str) const
		{
			if (_data.get() == str._data.get())
				return 0;
			size_t lsize = size();
			size_t rsize = str.size();
			int res = std::char_traits<CharType>::compare(_data.get(), str._data.get(), std::min(lsize, rsize));
			if (res != 0)
				return res;
			if (lsize < rsize)
				return -1;
			if (lsize > rsize)
				return 1;
			return 0;
		}

		StringBase& replace(size_t pos, size_t count, const StringBase& str) nonconst THROW_STD_EXCEPT
		{
			_call_non_const_func();
			_range_check(pos);
			count = std::min({ str.size(), count, size() - pos });
			helpers::copy_str(_data.get() + pos, str._data.get(), count);
			return *this;
		}

		void swap(StringBase& other)nonconst THROW_STD_EXCEPT
		{
			if (_data.get() == other._data.get()) return;
			_call_non_const_func();
			other._call_non_const_func();
			std::swap(_size, other._size);
			std::swap(_capacity, other._capacity);
			std::swap(_cur_arg_index, other._cur_arg_index);
			_data.swap(other._data);
		}

		StringBase substr(size_t pos = 0, size_t count = npos)const THROW_STD_EXCEPT
		{
			StringBase str;
			if (_size == 0 || count == 0) return str;
			str.append(*this, pos, count);
			return str;
		}

		template<typename V>
		StringBase arg(V value) const THROW_STD_EXCEPT
		{
			int index = _cur_arg_index;
			StringBase str;
			CharType begin_c[3] = { '{', '%', '\0' };
			CharType end_c[2] = { '}', '\0' };
			StringBase fmt_begin(begin_c);
			StringBase fmt_end(end_c);
			size_t fmt_tag_begin, fmt_tag_end;
			while (index < 100)
			{
				StringBase si = to_str(index);
				str = fmt_begin + si + fmt_end;
				fmt_tag_begin = find(str);
				if (fmt_tag_begin != npos)
				{
					fmt_tag_end = fmt_tag_begin + str.size();
					break;
				}
				index++;
			}
			if (index == 100) return *this;
			str = StringBase(_data.get(), fmt_tag_begin);
			str += to_str(value);
			str += StringBase(_data.get() + fmt_tag_end, _size - fmt_tag_end);
			str._cur_arg_index = index + 1;
			return str;
		}

	public: //operators
		CharReference operator[](size_t index) nonconst THROW_STD_EXCEPT
		{
			_call_non_const_func();
			_range_check(index);
			return *(_data.get() + index);
		}

		StringBase& operator+=(const StringBase& str) nonconst THROW_STD_EXCEPT
		{
			_call_non_const_func();
			append(str);
			return *this;
		}

		StringBase& operator+=(CharType ch) nonconst THROW_STD_EXCEPT
		{
			_call_non_const_func();
			append(ch);
			return *this;
		}

		StringBase& operator=(const StringBase& other) nonconst THROW_STD_EXCEPT
		{
			reset();
			_data = other._data;
			_size = other._size;
			_capacity = other._capacity;
			return *this;
		}

	public: //Search
		size_t find(const StringBase& str, size_t pos = 0) const
		{
			auto s = c_str();
			auto t = str.c_str();
			CharPtrType res = helpers::strstr<CharType>(s.get() + pos, t.get());
			if (res == nullptr)
				return npos;
			else
				return res - s.get();
		}

		size_t find(const CharType* str, size_t pos, size_t count) const
		{
			auto s = c_str();
			auto t = StringBase(str, count).c_str();
			CharPtrType res = helpers::strstr<CharType>(s.get() + pos, t.get());
			if (res == nullptr)
				return npos;
			else
				return res - s.get();
		}

		size_t find_first_of(const StringBase& str, size_t pos = 0)const THROW_STD_EXCEPT
		{
			auto it = std::find_first_of(begin() + pos, end(), str.begin(), str.end());
			if (it == end())
				return npos;
			else
				return std::distance(begin(), it);
		}

		size_t find_first_of(CharType ch, size_t pos = 0)const THROW_STD_EXCEPT
		{
			auto it = std::find(begin() + pos, end(), ch);
			if (it == end())
				return npos;
			else
				return std::distance(begin(), it);
		}
	public: //Statics
		template<typename V>
		static StringBase to_str(V value);

	private:
		inline void _call_non_const_func() nonconst THROW_STD_EXCEPT
		{
			if (!_data.unique())
			{
				_data = helpers::get_copy_str<CharType>(_data.get(), _size, false);
			}
		}

		size_t _get_expand_size(size_t increment) const
		{
			if (_size + increment < _capacity) return _capacity;
			else if (_size == 0) return increment;
			size_t new_capacity = _capacity;
			while (new_capacity < _size + increment)
			{
				new_capacity = static_cast<size_t>(new_capacity * _expand_proportion + 0.5);
			}
			return new_capacity;
		}

		INLINE void _range_check(int index) const THROW_STD_EXCEPT
		{
			helpers::expect<errors::OutOfRange>(index < static_cast<int>(_size) && index >= 0);
		}
	private:
		CharSharedPtrType _data;
		size_t _size;
		size_t _capacity;
		size_t _cur_arg_index = 1;
		const double _expand_proportion = 1.5;
		static_assert(std::is_same<CharType, char>::value || std::is_same<CharType, wchar_t>::value, "unsupported type: Char.");
	public:
		static const size_t npos = static_cast<size_t>(-1);
	};

	template<typename Char>
	template<typename V>
	StringBase<Char> StringBase<Char>::to_str(V value)
	{
		StringBase str(value);
		return str;
	}

	template<typename Char>
	std::basic_ostream<Char>& operator<<(std::basic_ostream<Char>& os, const StringBase<Char>& s)
	{
		for (size_t i = 0; i < s.size(); i++)
			os << s.at(i);
		return os;
	}

	template<typename Char>
	StringBase<Char> operator+(const StringBase<Char>& lhs, const StringBase<Char>& rhs) THROW_STD_EXCEPT
	{
		auto res = lhs;
		res.append(rhs);
		return res;
	}

	template<typename Char>
	StringBase<Char> operator+(const StringBase<Char>& lhs, Char rhs) THROW_STD_EXCEPT
	{
		auto res = lhs;
		res.append(rhs);
		return res;
	}

	template<typename Char>
	StringBase<Char> operator+(Char lhs, const StringBase<Char>& rhs) THROW_STD_EXCEPT
	{
		StringBase<Char> res;
		res.append(lhs);
		res.append(rhs);
		return res;
	}

	template<typename Char>
	bool operator==(const StringBase<Char>& lhs, const StringBase<Char>& rhs) THROW_STD_EXCEPT
	{
		return lhs.compare(rhs) == 0;
	}

	template<typename Char>
	bool operator!=(const StringBase<Char>& lhs, const StringBase<Char>& rhs) THROW_STD_EXCEPT
	{
		return !(lhs == rhs);
	}

	template<typename Char>
	bool operator<(const StringBase<Char>& lhs, const StringBase<Char>& rhs) THROW_STD_EXCEPT
	{
		return lhs.compare(rhs) < 0;
	}

	template<typename Char>
	bool operator<=(const StringBase<Char>& lhs, const StringBase<Char>& rhs) THROW_STD_EXCEPT
	{
		return lhs.compare(rhs) <= 0;
	}

	template<typename Char>
	bool operator>(const StringBase<Char>& lhs, const StringBase<Char>& rhs) THROW_STD_EXCEPT
	{
		return lhs.compare(rhs) > 0;
	}

	template<typename Char>
	bool operator>=(const StringBase<Char>& lhs, const StringBase<Char>& rhs) THROW_STD_EXCEPT
	{
		return lhs.compare(rhs) >= 0;
	}

	using String = StringBase<char>;
	using WString = StringBase<wchar_t>;

} //namespace cildhdi

#endif