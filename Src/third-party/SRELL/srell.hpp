/*****************************************************************************
**
**  SRELL (std::regex-like library) version 1.140
**
**  Copyright (c) 2012-2015, Nozomu Katô. All rights reserved.
**
**  Redistribution and use in source and binary forms, with or without
**  modification, are permitted provided that the following conditions are
**  met:
**
**  1. Redistributions of source code must retain the above copyright notice,
**     this list of conditions and the following disclaimer.
**
**  2. Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in the
**     documentation and/or other materials provided with the distribution.
**
**  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
**  IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
**  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
**  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
**  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
**  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
**  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
**  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
**  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
**  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
******************************************************************************
**/

#ifndef SRELL_REGEX_TEMPLATE_LIBRARY
#define SRELL_REGEX_TEMPLATE_LIBRARY

#include <cassert>
#include <climits>
#include <cwchar>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <utility>
#include <locale>
#include <vector>
#include <bitset>
#include <iterator>
#include <memory>
#include <algorithm>

#if defined(__cplusplus) && __cplusplus >= 201103L
  #ifndef SRELL_CPP11_CHAR1632_ENABLED
    #define SRELL_CPP11_CHAR1632_ENABLED
  #endif
  #ifndef SRELL_CPP11_INITIALIZER_LIST_ENABLED
    #include <initializer_list>
    #define SRELL_CPP11_INITIALIZER_LIST_ENABLED
  #endif
  #ifndef SRELL_CPP11_MOVE_ENABLED
    #define SRELL_CPP11_MOVE_ENABLED
  #endif
#endif

namespace srell
{
//  ["regex_constants.h" ...

	namespace regex_constants
	{
		typedef unsigned int bitmask_type;
	}

	namespace regex_constants
	{
		typedef bitmask_type syntax_option_type;

		static const syntax_option_type icase      = 1 << 0;
		static const syntax_option_type nosubs     = 1 << 1;
		static const syntax_option_type optimize   = 1 << 2;
		static const syntax_option_type collate    = 1 << 3;
		static const syntax_option_type ECMAScript = 1 << 4;
		static const syntax_option_type basic      = 1 << 5;
		static const syntax_option_type extended   = 1 << 6;
		static const syntax_option_type awk        = 1 << 7;
		static const syntax_option_type grep       = 1 << 8;
		static const syntax_option_type egrep      = 1 << 9;

		static const syntax_option_type multiline  = 1 << 10;
	}
	//  namespace regex_constants

	namespace regex_constants
	{
		typedef bitmask_type match_flag_type;

		static const match_flag_type match_default     = 0;
		static const match_flag_type match_not_bol     = 1 <<  0;
		static const match_flag_type match_not_eol     = 1 <<  1;
		static const match_flag_type match_not_bow     = 1 <<  2;
		static const match_flag_type match_not_eow     = 1 <<  3;
		static const match_flag_type match_any         = 1 <<  4;
		static const match_flag_type match_not_null    = 1 <<  5;
		static const match_flag_type match_continuous  = 1 <<  6;
		static const match_flag_type match_prev_avail  = 1 <<  7;

		static const match_flag_type format_default    = 0;
		static const match_flag_type format_sed        = 1 <<  8;
		static const match_flag_type format_no_copy    = 1 <<  9;
		static const match_flag_type format_first_only = 1 << 10;
	}
	//  namespace regex_constants

	//  28.5, regex constants:
	namespace regex_constants
	{
//		typedef implementation defined error_type;
		typedef int error_type;

		static const error_type error_collate    = 100;
		static const error_type error_ctype      = 101;
		static const error_type error_escape     = 102;
		static const error_type error_backref    = 103;
		static const error_type error_brack      = 104;
		static const error_type error_paren      = 105;
		static const error_type error_brace      = 106;
		static const error_type error_badbrace   = 107;
		static const error_type error_range      = 108;
		static const error_type error_space      = 109;
		static const error_type error_badrepeat  = 110;
		static const error_type error_complexity = 111;
		static const error_type error_stack      = 112;

		static const error_type error_lookbehind = 200;
		static const error_type error_internal   = 999;
	}
	//  namespace regex_constants

//  ... "regex_constants.h"]
//  ["rei_type.h" ...

	namespace regex_internal
	{

#if defined(SRELL_CPP11_CHAR1632_ENABLED)

		typedef char32_t uchar21_t;

#else	//  !defined(SRELL_CPP11_CHAR1632_ENABLED)

  #if defined(WCHAR_MAX) && WCHAR_MAX >= 0x10ffff

		typedef wchar_t uchar21_t;

  #elif defined(UINT_MAX) && UINT_MAX >= 0x10ffff

		typedef unsigned int uchar21_t;

  #elif defined(ULONG_MAX) && ULONG_MAX >= 0x10ffff

		typedef unsigned long uchar21_t;

  #else
  #error could not find a suitable type for 21-bit Unicode integers.
  #endif
#endif	//  SRELL_CPP11_CHAR1632_ENABLED

	}	//  regex_internal

//  ... "rei_type.h"]
//  ["rei_constants.h" ...

	namespace regex_internal
	{
		enum re_state_type
		{
			st_character,               //  0x00
			st_character_class,         //  0x01

			st_epsilon,                 //  0x02

			st_check_counter,           //  0x03
			st_increment_counter,       //  0x04
			st_decrement_counter,       //  0x05
			st_save_and_reset_counter,  //  0x06
			st_restore_counter,         //  0x07

			st_roundbracket_open,       //  0x08
			st_roundbracket_pop,        //  0x09
			st_roundbracket_close,      //  0x0a

			st_repeat_in_push,          //  0x0b
			st_repeat_in_pop,           //  0x0c
			st_check_0_width_repeat,    //  0x0d

			st_backreference,           //  0x0e

			st_lookaround_open,         //  0x0f

			st_lookaround_pop,          //  0x10

			st_bol,                     //  0x11
			st_eol,                     //  0x12
			st_boundary,                //  0x13

			st_success,                 //  0x14

#if !defined(SRELLDBG_NO_NEXTPOS_OPT)
			st_move_nextpos,            //  0x15
#endif

			st_lookaround_close        = st_success,
			st_zero_width_boundary     = st_lookaround_open,
			//st_assertions_boundary     = st_lookaround_open,

//			st_independent_open,
//			st_previous_match,
//			st_independent_close,       //  = st_success,
		};
		//  re_state_type

		namespace constants
		{
			static const uchar21_t unicode_max_codepoint = 0x10ffff;
			static const uchar21_t unicode_codepoint_end = 0x110000;
			static const uchar21_t invalid_u21value = static_cast<uchar21_t>(~0);
		}
		//  constants

		namespace meta_char
		{
			static const uchar21_t mc_exclam = 0x21;	//  '!'
			static const uchar21_t mc_sharp  = 0x23;	//  '#'
			static const uchar21_t mc_dollar = 0x24;	//  '$'
			static const uchar21_t mc_rbraop = 0x28;	//  '('
			static const uchar21_t mc_rbracl = 0x29;	//  ')'
			static const uchar21_t mc_astrsk = 0x2a;	//  '*'
			static const uchar21_t mc_plus   = 0x2b;	//  '+'
			static const uchar21_t mc_comma  = 0x2c;	//  ','
			static const uchar21_t mc_minus  = 0x2d;	//  '-'
			static const uchar21_t mc_period = 0x2e;	//  '.'
			static const uchar21_t mc_colon  = 0x3a;	//  ':'
			static const uchar21_t mc_lt = 0x3c;		//  '<'
			static const uchar21_t mc_eq = 0x3d;		//  '='
			static const uchar21_t mc_gt = 0x3e;		//  '>'
			static const uchar21_t mc_query  = 0x3f;	//  '?'
			static const uchar21_t mc_sbraop = 0x5b;	//  '['
			static const uchar21_t mc_escape = 0x5c;	//  '\\'
			static const uchar21_t mc_sbracl = 0x5d;	//  ']'
			static const uchar21_t mc_caret  = 0x5e;	//  '^'
			static const uchar21_t mc_cbraop = 0x7b;	//  '{'
			static const uchar21_t mc_bar    = 0x7c;	//  '|'
			static const uchar21_t mc_cbracl = 0x7d;	//  '}'
		}
		//  meta_char

		namespace char_ctrl
		{
			static const uchar21_t cc_nul  = 0x00;	//  '\0'	//0x00:NUL
			static const uchar21_t cc_bell = 0x07;	//  '\a'	//0x07:BEL
			static const uchar21_t cc_bs   = 0x08;	//  '\b'	//0x08:BS
			static const uchar21_t cc_htab = 0x09;	//  '\t'	//0x09:HT
			static const uchar21_t cc_nl   = 0x0a;	//  '\n'	//0x0a:LF
			static const uchar21_t cc_vtab = 0x0b;	//  '\v'	//0x0b:VT
			static const uchar21_t cc_ff   = 0x0c;	//  '\f'	//0x0c:FF
			static const uchar21_t cc_cr   = 0x0d;	//  '\r'	//0x0d:CR
			static const uchar21_t cc_esc  = 0x1b;	//  '\x1b'	//0x1b:ESC
		}
		//  char_ctrl

		namespace char_alnum
		{
			static const uchar21_t ch_0 = 0x30;	//  '0'
			static const uchar21_t ch_1 = 0x31;	//  '1'
			static const uchar21_t ch_7 = 0x37;	//  '7'
			static const uchar21_t ch_8 = 0x38;	//  '8'
			static const uchar21_t ch_9 = 0x39;	//  '9'
			static const uchar21_t ch_A = 0x41;	//  'A'
			static const uchar21_t ch_B = 0x42;	//  'B'
			static const uchar21_t ch_D = 0x44;	//  'D'
			static const uchar21_t ch_F = 0x46;	//  'F'
			static const uchar21_t ch_G = 0x47;	//  'G'
			static const uchar21_t ch_P = 0x50;	//  'P'
			static const uchar21_t ch_S = 0x53;	//  'S'
			static const uchar21_t ch_U = 0x55;	//  'U'
			static const uchar21_t ch_W = 0x57;	//  'W'
			static const uchar21_t ch_Z = 0x5a;	//  'Z'
			static const uchar21_t ch_a = 0x61;	//  'a'
			static const uchar21_t ch_b = 0x62;	//  'b'
			static const uchar21_t ch_c = 0x63;	//  'c'
			static const uchar21_t ch_d = 0x64;	//  'd'
			static const uchar21_t ch_e = 0x65;	//  'e'
			static const uchar21_t ch_f = 0x66;	//  'f'
			static const uchar21_t ch_i = 0x69;	//  'i'
			static const uchar21_t ch_m = 0x6d;	//  'm'
			static const uchar21_t ch_n = 0x6e;	//  'n'
			static const uchar21_t ch_o = 0x6f;	//  'o'
			static const uchar21_t ch_p = 0x70;	//  'p'
			static const uchar21_t ch_r = 0x72;	//  'r'
			static const uchar21_t ch_s = 0x73;	//  's'
			static const uchar21_t ch_t = 0x74;	//  't'
			static const uchar21_t ch_u = 0x75;	//  'u'
			static const uchar21_t ch_v = 0x76;	//  'v'
			static const uchar21_t ch_w = 0x77;	//  'w'
			static const uchar21_t ch_x = 0x78;	//  'x'
			static const uchar21_t ch_z = 0x7a;	//  'z'
		}
		//  char_alnum

		namespace char_other
		{
			static const uchar21_t co_amp  = 0x26;	//  '&'
			static const uchar21_t co_apos = 0x27;	//  '\''
			static const uchar21_t co_grav = 0x60;	//  '`'
		}
		//  char_other

	}
	//  namespace regex_internal

	namespace regex_constants
	{
		//  For internal use.
		static const match_flag_type match_last_tried_ = 1 << 11;
		static const match_flag_type match_match_      = 1 << 12;
	}
	//  namespace regex_constants

//  ... "rei_constants.h"]
//  ["rei_memory.hpp" ...

	namespace regex_internal
	{

/*
 *  Similar to std::basic_string, except for:
 *    a. only allocates memory, does not initialise it.
 *    b. uses realloc() to avoid moving data as much as possible when
 *       resizing an allocated buffer.
 */
template <typename ElemT>
class simple_array
{
public:

	typedef ElemT value_type;
	typedef std::size_t size_type;
	typedef ElemT &reference;
	typedef const ElemT &const_reference;
	typedef ElemT *pointer;
	typedef const ElemT *const_pointer;

public:

	simple_array()
		: size_(0)
		, capacity_(0)
		, buffer_(NULL)
	{
	}

	simple_array(const size_type initsize, const value_type)
		: size_(initsize)
		, capacity_(initsize)
		, buffer_(static_cast<pointer>(std::malloc(initsize * sizeof (ElemT))))
	{
		if (buffer_ == NULL)
		{
			size_ = capacity_ = 0;
			throw std::bad_alloc();
		}
	}

	simple_array(const simple_array &right)
		: size_(0)
		, capacity_(0)
		, buffer_(NULL)
	{
		operator=(right);
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	simple_array(simple_array &&right) noexcept
		: size_(right.size_)
		, capacity_(right.capacity_)
		, buffer_(right.buffer_)
	{
		right.size_ = 0;
		right.capacity_ = 0;
		right.buffer_ = NULL;
	}
#endif

	simple_array &operator=(const simple_array &right)
	{
		if (this != &right)
		{
			resize(right.size_);
			for (size_type i = 0; i < right.size_; ++i)
				buffer_[i] = right.buffer_[i];
		}
		return *this;
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	simple_array &operator=(simple_array &&right) noexcept
	{
		if (this != &right)
		{
			this->size_ = right.size_;
			this->capacity_ = right.capacity_;
			this->buffer_ = right.buffer_;

			right.size_ = 0;
			right.capacity_ = 0;
			right.buffer_ = NULL;
		}
		return *this;
	}
#endif

	~simple_array()
	{
		if (buffer_ != NULL)
			std::free(buffer_);
	}

	size_type size() const
	{
		return size_;
	}

	void clear()
	{
		size_ = 0;
	}

	bool resize(const size_type newsize)
	{
		if (newsize <= capacity_ || reserve(newsize))
		{
			size_ = newsize;
			return true;
		}
		return false;
	}

	bool resize(const size_type newsize, const ElemT &type)
	{
		size_type oldsize = size_;

		if (resize(newsize))
		{
			for (; oldsize < size_; ++oldsize)
				buffer_[oldsize] = type;

			return true;
		}
		return false;
	}

	reference operator[](const size_type pos)
	{
		return buffer_[pos];
	}

	const_reference operator[](const size_type &pos) const
	{
		return buffer_[pos];
	}

	bool push_back(const_reference n)
	{
		const size_type newsize = size_ + 1;

		if (newsize <= capacity_ || reserve(newsize))
		{
			buffer_[size_] = n;
			++size_;
			return true;
		}
		return false;
	}

	const_reference back() const
	{
		return buffer_[size_ - 1];
	}

	reference back()
	{
		return buffer_[size_ - 1];
	}

	void pop_back()
	{
		--size_;
	}

	simple_array &operator+=(const simple_array &right)
	{
		size_type oldsize = size_;
		resize(size_ + right.size_);

		for (size_type i = 0; i < right.size_; ++i, ++oldsize)
			buffer_[oldsize] = right.buffer_[i];

		return *this;
	}

	//  For regex_char_class class.
	void erase(const size_type pos)
	{
		if (pos < size_)
		{
			std::memcpy(buffer_ + pos, buffer_ + pos + 1, (size_ - pos - 1) * sizeof (ElemT));
			--size_;
		}
	}

	//  For regex_compiler class.
	void insert(const size_type pos, const ElemT &type)
	{
		move_forward(pos, 1);
		buffer_[pos] = type;
	}

	void insert(size_type pos, const simple_array &right)
	{
		move_forward(pos, right.size_);
		for (size_type i = 0; i < right.size_; ++i, ++pos)
			buffer_[pos] = right.buffer_[i];
	}

	bool reserve(const size_type newsize)
	{
		if (newsize > capacity_)
		{
//			const size_type newcapacity = newsize + (newsize >> 1);	//  newsize * 1.5.
			const size_type newcapacity = ((newsize >> 8) + 1) << 8;	//  round up to a multiple of 256.
			const size_type size_in_byte = newcapacity * sizeof (ElemT);

#if 0
			if (size_in_byte < capacity_ * sizeof (ElemT))	//  overflow (carry).
				throw std::bad_alloc();
#endif

			const pointer newbuffer = static_cast<pointer>(std::realloc(buffer_, size_in_byte));

			if (newbuffer == NULL)
			{
				//  Even if realloc() failed, already-existing buffer remains valid.
				std::free(buffer_);
				buffer_ = NULL;
				size_ = capacity_ = 0;
				throw std::bad_alloc();
			}

			buffer_ = newbuffer;
			capacity_ = newcapacity;
			return true;
		}
		return true;
	}

private:

	void move_forward(const size_type pos, const size_type count)
	{
		size_type oldsize = size_;

		resize(size_ + count);

		const pointer begin = buffer_ + pos;
		pointer src = buffer_ + oldsize;
		pointer dest = src + count;

		while (src > begin)
			*--dest = *--src;
	}

private:

	size_type size_;
	size_type capacity_;
	pointer buffer_;
};

	}	//  namespace regex_internal

//  ... "rei_memory.hpp"]
//  ["rei_ucf.hpp" ...

	namespace regex_internal
	{

#include "srell_ucfdata.hpp"

class unicode_case_folding
{
public:

	typedef regex_internal::uchar21_t uchar21_t;
	typedef std::basic_string<uchar21_t> u21string;

	unicode_case_folding()
	{
		if (general_table_ptr()[0] == 0)
			make_tables();	//  No matter how many threads call it at the same time.
	}

	uchar21_t do_casefolding(const uchar21_t &cp) const
	{
		if (cp <= ucf_data::ucf_maxcodepoint)
			return cp + ucf_lower_table_ptr()[ucf_upper_table_ptr()[cp >> 8] + (cp & 0xff)];

		return cp;
	}

	u21string casefoldedcharset(const uchar21_t &cp) const
	{
		if (cp <= ucf_data::rev_maxcodepoint)
		{
			const uchar21_t offset_of_charset = rev_lower_table_ptr()[rev_upper_table_ptr()[cp >> 8] + (cp & 0xff)];
			const uchar21_t *ptr = &rev_charset_table_ptr()[offset_of_charset];
			const uchar21_t *const begin = ptr;

			for (; *ptr != cfcharset_eos_; ++ptr);

			if (begin != ptr)
				return u21string(begin, ptr);
		}

		return u21string(1, cp);
	}

	unicode_case_folding &operator=(const unicode_case_folding &)
	{
		return *this;
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	unicode_case_folding &operator=(unicode_case_folding &&)
	{
		return *this;
	}
#endif

private:

	struct uchar21_pair
	{
		uchar21_t from;
		uchar21_t to;
	};

	uchar21_t *general_table_ptr() const
	{
		static uchar21_t general_table[gentable_size_];

		return general_table;
	}

	//  tables for Unicode case folding.
	const uchar21_t *ucf_upper_table_ptr() const
	{
//		static uchar21_t ucf_upper_table[(ucf_data::ucf_maxcodepoint >> 8) + 1];

//		return ucf_upper_table;
		return general_table_ptr() + ucf_upper_table_base_;
	}
	const uchar21_t *ucf_lower_table_ptr() const
	{
//		static uchar21_t ucf_lower_table[ucf_data::ucf_deltatablesize];

//		return ucf_lower_table;
		return general_table_ptr() + ucf_lower_table_base_;
	}

	//  tables for reverse lookup.
	const uchar21_t *rev_upper_table_ptr() const
	{
//		static uchar21_t rev_upper_table[(ucf_data::rev_maxcodepoint >> 8) + 1];

//		return rev_upper_table;
		return general_table_ptr() + rev_upper_table_base_;
	}
	const uchar21_t *rev_lower_table_ptr() const
	{
//		static uchar21_t rev_lower_table[ucf_data::rev_indextablesize];

//		return rev_lower_table;
		return general_table_ptr() + rev_lower_table_base_;
	}
	const uchar21_t *rev_charset_table_ptr() const
	{
//		static uchar21_t rev_charset_table[ucf_data::rev_charsettablesize];

//		return rev_charset_table;
		return general_table_ptr() + rev_charset_table_base_;
	}

	bool make_tables()
	{
		//  This function can be called by multiple threads at the same
		//  time. So, we create lookup tables as local objects first,
		//  then copy them into general_table.
		//  As of Unicode 7.0.0, gentable_size_ is 13561 = 54244 bytes.
		const uchar21_pair *const ucf_pairs = ucf_data::table();
		uchar21_t localgentable[gentable_size_];	//  should be put in the heap instead of the stack?
		uchar21_t *const ucf_deltatable = localgentable + ucf_lower_table_base_;	//  ucf_lower_table_ptr();
		uchar21_t *const ucf_segtable = localgentable + ucf_upper_table_base_;	//  ucf_upper_table_ptr();
		uchar21_t *const rev_indextable = localgentable + rev_lower_table_base_;	//  rev_lower_table_ptr();
		uchar21_t *const rev_segtable = localgentable + rev_upper_table_base_;	//  rev_upper_table_ptr();
		uchar21_t *const rev_cstable = localgentable + rev_charset_table_base_;	//  rev_charset_table_ptr();
		uchar21_t rev_cstable_size = 0;
		uchar21_t nextoffset = 0x100;

		std::memset(localgentable, 0, gentable_size_ * sizeof (uchar21_t));
		rev_cstable[rev_cstable_size++] = cfcharset_eos_;

		//  create ucf_segment_table and rev_charset_table.
		for (const uchar21_pair *cfp = ucf_pairs; cfp->from; ++cfp)
		{
			uchar21_t &offset_of_segment = ucf_segtable[cfp->from >> 8];

			if (offset_of_segment == 0)
			{
				offset_of_segment = nextoffset;
				nextoffset += 0x100;
			}

			for (uchar21_t index = 0;; ++index)
			{
				if (index == rev_cstable_size)
				{
					rev_cstable[rev_cstable_size++] = cfp->to;
					rev_cstable[rev_cstable_size++] = cfp->from;
					rev_cstable[rev_cstable_size++] = cfcharset_eos_;
					break;
				}
				if (rev_cstable[index] == cfp->to)
				{
					for (++index; rev_cstable[index] != cfcharset_eos_; ++index);

					std::memmove(&rev_cstable[index + 1], &rev_cstable[index], (rev_cstable_size - index) * sizeof(uchar21_t));
					rev_cstable[index] = cfp->from;
					++rev_cstable_size;
					break;
				}
			}
		}

#if defined(SRELLDBG_RANGECHECK)
		std::fprintf(stdout, "ucf_deltatablesize: %x/%x\n", nextoffset, ucf_data::ucf_deltatablesize);	//  DEBUG.
		std::fprintf(stdout, "rev_cstable_size: %u/%u\n", rev_cstable_size, ucf_data::rev_charsettablesize);	//  DEBUG.
#endif

		//  create ucf_delta_table.
		for (const uchar21_pair *cfp = ucf_pairs; cfp->from; ++cfp)
		{
			const uchar21_t &offset_of_segment = ucf_segtable[cfp->from >> 8];

			ucf_deltatable[offset_of_segment + (cfp->from & 0xff)] = static_cast<uchar21_t>(cfp->to - cfp->from);
		}

		//  create rev_segment_table and rev_index_table.
		nextoffset = 0x100;
		for (uchar21_t index = 0; index < rev_cstable_size; ++index)
		{
			const uchar21_t bocs = index;	//  begenning of charset.

			for (; rev_cstable[index] != cfcharset_eos_; ++index)
			{
				const uchar21_t &u21ch = rev_cstable[index];
				uchar21_t &offset_of_segment = rev_segtable[u21ch >> 8];

				if (offset_of_segment == 0)
				{
					offset_of_segment = nextoffset;
					nextoffset += 0x100;
				}
				rev_indextable[offset_of_segment + (u21ch & 0xff)] = bocs;
			}
		}

#if defined(SRELLDBG_RANGECHECK)
		std::fprintf(stdout, "rev_indextablesize: %x/%x\n", nextoffset, ucf_data::rev_indextablesize);	//  DEBUG.
#endif

		std::memcpy(reinterpret_cast<char *>(general_table_ptr()) + 1, reinterpret_cast<const char *>(localgentable) + 1, gentable_size_ * sizeof (uchar21_t) - 1);
		std::memcpy(general_table_ptr(), localgentable, 1);	//  tells that the table is ready.

		return true;
	}

	typedef unicode_casefolding<uchar21_pair, uchar21_t, uchar21_t> ucf_data;
	static const uchar21_t cfcharset_eos_ = static_cast<uchar21_t>(-1);

	static const std::size_t gentable_size_
		= (ucf_data::ucf_maxcodepoint >> 8) + 1 //  for ucf-upper.
		+ ucf_data::ucf_deltatablesize          //  for ucf_lower.
		+ (ucf_data::rev_maxcodepoint >> 8) + 1 //  for rev-upper.
		+ ucf_data::rev_indextablesize          //  for rev-lower.
		+ ucf_data::rev_charsettablesize;       //  for rev-lookup.

	static const std::size_t ucf_upper_table_base_ = 0;
	static const std::size_t ucf_lower_table_base_ = ucf_upper_table_base_ + (ucf_data::ucf_maxcodepoint >> 8) + 1;
	static const std::size_t rev_upper_table_base_ = ucf_lower_table_base_ + ucf_data::ucf_deltatablesize;
	static const std::size_t rev_lower_table_base_ = rev_upper_table_base_ + (ucf_data::rev_maxcodepoint >> 8) + 1;
	static const std::size_t rev_charset_table_base_ = rev_lower_table_base_ + ucf_data::rev_indextablesize;
};

	}	//  namespace regex_internal

//  ... "rei_ucf.hpp"]
//  ["regex_error.hpp" ...

//  28.6, class regex_error:
class regex_error : public std::runtime_error
{
public:

	explicit regex_error(const regex_constants::error_type ecode)
		: std::runtime_error("regex_error")	//  added for error C2512.
		, ecode_(ecode)
	{
	}

	regex_constants::error_type code() const
	{
		return ecode_;
	}

private:

	regex_constants::error_type ecode_;
};

//  ... "regex_error.hpp"]
//  ["regex_traits.hpp" ...

	namespace regex_internal
	{

//  28.7, class template regex_traits:
template <class charT>
struct traits_core
{
public:

	typedef charT char_type;
	typedef std::basic_string<char_type> string_type;
	typedef std::locale locale_type;
//	typedef bitmask_type char_class_type;
	typedef unsigned int char_class_type;

public:

//	regex_traits();

	static std::size_t length(const char_type *p)
	{
		return std::char_traits<charT>::length(p);
	}

	charT translate(const charT c) const
	{
		return c;
	}

	charT translate_nocase(const charT c) const
	{
		return c;
	}

	template <class ForwardIterator>
	string_type transform(ForwardIterator first, ForwardIterator last) const
	{
		return string_type(first, last);
	}

	template <class ForwardIterator>
	string_type transform_primary(ForwardIterator first, ForwardIterator last) const
	{
		return string_type(first, last);
	}

	template <class ForwardIterator>
	string_type lookup_collatename(ForwardIterator first, ForwardIterator last) const
	{
		return string_type(first, last);
	}

	template <class ForwardIterator>
	char_class_type lookup_classname(ForwardIterator /* first */, ForwardIterator /* last */, bool /* icase */ = false) const
	{
		return static_cast<char_class_type>(0);
	}

	bool isctype(const charT /* c */, const char_class_type /* f */) const
	{
		return false;
	}

	int value(const charT ch, const int radix) const
	{
		if ((ch >= static_cast<charT>(char_alnum::ch_0) && ch <= static_cast<charT>(char_alnum::ch_7)) || (radix >= 10 && (ch == static_cast<charT>(char_alnum::ch_8) || ch == static_cast<charT>(char_alnum::ch_9))))
			return static_cast<int>(ch - char_alnum::ch_0);

		if (radix == 16)
		{
			if (ch >= static_cast<charT>(char_alnum::ch_a) && ch <= static_cast<charT>(char_alnum::ch_f))
				return static_cast<int>(ch - char_alnum::ch_a + 10);

			if (ch >= static_cast<charT>(char_alnum::ch_A) && ch <= static_cast<charT>(char_alnum::ch_F))
				return static_cast<int>(ch - char_alnum::ch_A + 10);
		}
		return -1;
	}

	locale_type imbue(const locale_type /* l */)
	{
		return locale_type();
	}

	locale_type getloc() const
	{
		return locale_type();
	}

	//  Additional members for Unicode support.

	static const std::size_t maxseqlen = 1;

	uchar21_t unicode_casefolding(const uchar21_t &c) const
	{
#if !defined(SRELL_NO_UNICODE_ICASE)
		return ucf_.do_casefolding(c);
#else
		if (c >= char_alnum::ch_A && c <= char_alnum::ch_Z)	//  'A' && 'Z'
			return static_cast<uchar21_t>(c - char_alnum::ch_A + char_alnum::ch_a);	//  - 'A' + 'a'

		return c;
#endif
	}

	std::basic_string<uchar21_t> unicode_reverse_casefolding(const uchar21_t &cp) const
	{
#if !defined(SRELL_NO_UNICODE_ICASE)
		return ucf_.casefoldedcharset(cp);
#else
//		const uchar21_t nocase = static_cast<uchar21_t>(cp & ~0x20);
		const uchar21_t nocase = static_cast<uchar21_t>(cp | 0x20);
		std::basic_string<uchar21_t> table;

		table.push_back(cp);
//		if (nocase >= char_alnum::ch_A && nocase <= char_alnum::ch_Z)
		if (nocase >= char_alnum::ch_a && nocase <= char_alnum::ch_z)
			table.push_back(static_cast<uchar21_t>(cp ^ 0x20));

		return table;
#endif
	}

	//  *iter
	template <typename InputIterator>
	uchar21_t codepoint(InputIterator begin, const InputIterator /* end */) const
	{
		return static_cast<uchar21_t>(*begin);
		//  Caller is responsible for begin != end.
	}

	//  *iter++
	template <typename InputIterator>
	uchar21_t codepoint_inc(InputIterator &begin, const InputIterator /* end */) const
	{
		return static_cast<uchar21_t>(*begin++);
		//  Caller is responsible for begin != end.
	}

	//  *--iter
	template <typename BidirectionalIterator>
	uchar21_t prevcodepoint(BidirectionalIterator cur) const
	{

		return static_cast<uchar21_t>(*--cur);
		//  No range check is done on purpose. Since this function is called
		//  also when regex_constants::match_prev_avail is specified, it is
		//  possible to exceed [begin, end).
	}

#if !defined(SRELL_NO_REVERSE)

	//  *--iter
	template <typename BidirectionalIterator>
	uchar21_t dec_codepoint(BidirectionalIterator &cur, const BidirectionalIterator /* begin */) const
	{
		return static_cast<uchar21_t>(*--cur);
		//  Caller is responsible for cur != begin.
	}

#endif	//  SRELL_NO_REVERSE

	traits_core &operator=(const traits_core &that)
	{
#if !defined(SRELL_NO_UNICODE_ICASE)
		this->ucf_ = that.ucf_;
#endif
		return *this;
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	traits_core &operator=(traits_core &&that)
	{
#if !defined(SRELL_NO_UNICODE_ICASE)
		this->ucf_ = std::move(that.ucf_);
#endif
		return *this;
	}
#endif

	//  TODO: switch function for case-folding of 'i'.

private:

#if !defined(SRELL_NO_UNICODE_ICASE)
	regex_internal::unicode_case_folding ucf_;
#endif

};		//  traits_core
	}	//  regex_internal

//  common and utf-32.
template <class charT>
struct regex_traits : public regex_internal::traits_core<charT>
{
private:

	typedef regex_internal::traits_core<charT> base_type;

public:

	typedef regex_internal::uchar21_t uchar21_t;
	typedef typename base_type::char_type char_type;
	typedef typename base_type::string_type string_type;
	typedef typename base_type::locale_type locale_type;
	typedef typename base_type::char_class_type char_class_type;
};

//  utf-8 specific.
template <class charT>
struct u8regex_traits : public regex_internal::traits_core<charT>
{
private:

	typedef regex_internal::traits_core<charT> base_type;

public:

	typedef regex_internal::uchar21_t uchar21_t;
	typedef typename base_type::char_type char_type;
	typedef typename base_type::string_type string_type;
	typedef typename base_type::locale_type locale_type;
	typedef typename base_type::char_class_type char_class_type;

	//  utf-8 specific.
	static const std::size_t maxseqlen = 4;

	template <typename InputIterator>
	uchar21_t codepoint(InputIterator begin, const InputIterator end) const
	{
		return codepoint_inc(begin, end);
	}

	template <typename InputIterator>
	uchar21_t codepoint_inc(InputIterator &begin, const InputIterator end) const
	{
		uchar21_t codepoint = static_cast<uchar21_t>(*begin++);

		if ((codepoint & 0x80) == 0)	//  1 octet
		{
			return static_cast<uchar21_t>(codepoint & 0x7f);
		}
		else if ((codepoint & 0xe0) == 0xc0)	//  2 octets
		{
			//  c080-c1bf: invalid.
			//  c280-dfbf: valid.

			codepoint = static_cast<uchar21_t>((codepoint & 0x1f) << 6);
			if (begin != end)
				codepoint = static_cast<uchar21_t>(codepoint | (*begin++ & 0x3f));
		}
		else if ((codepoint & 0xf0) == 0xe0)	//  3 octets
		{
			//  e08080-e09fbf: invalid.
			//  e0a080-efbfbf: valid.

			codepoint = static_cast<uchar21_t>((codepoint & 0x0f) << 12);
			if (begin != end)
			{
				codepoint = static_cast<uchar21_t>(codepoint | ((*begin & 0x3f) << 6));
				if (++begin != end)
					codepoint = static_cast<uchar21_t>(codepoint | (*begin++ & 0x3f));
			}
		}
		else
		{
			codepoint = static_cast<uchar21_t>(codepoint & 0xff);

			if (codepoint >= 0xf0 && codepoint <= 0xf4)	//  4 octets
			{
				//  f0808080-f08fbfbf: invalid.
				//  f0908080-f48fbfbf: valid.
				//  f4908080-f4bfbfbf: invalid.

				codepoint = static_cast<uchar21_t>((codepoint & 0x07) << 18);
				if (begin != end)
				{
					codepoint = static_cast<uchar21_t>(codepoint | ((*begin & 0x3f) << 12));
					if (++begin != end)
					{
						codepoint = static_cast<uchar21_t>(codepoint | ((*begin & 0x3f) << 6));
						if (++begin != end)
							codepoint = static_cast<uchar21_t>(codepoint | (*begin++ & 0x3f));
					}
				}
			}
			//else	//  80-bf, f5-ff: invalid.
		}
		return codepoint;
	}

	template <typename BidirectionalIterator>
	uchar21_t prevcodepoint(BidirectionalIterator cur) const
	{
		uchar21_t codepoint = 0;
		unsigned int shift = 0;

		for (std::size_t i = 0; i < maxseqlen; ++i)
		{
			--cur;
			if ((*cur & 0xc0) != 0x80)
			{
				if (i == 0)
					codepoint = static_cast<uchar21_t>(codepoint | ((*cur & 0x7f) << shift));	//  1 octet.
				else if (i == 1)
					codepoint = static_cast<uchar21_t>(codepoint | ((*cur & 0x1f) << shift));	//  2 octets.
				else if (i == 2)
					codepoint = static_cast<uchar21_t>(codepoint | ((*cur & 0x0f) << shift));	//  3 octets.
				else	//  if (i == 3)
					codepoint = static_cast<uchar21_t>(codepoint | ((*cur & 0x07) << shift));	//  4 octets.

				break;
			}
			codepoint = static_cast<uchar21_t>(codepoint | ((*cur & 0x3f) << shift));
			shift += 6;
		}
		return codepoint;
	}

#if !defined(SRELL_NO_REVERSE)

	template <typename BidirectionalIterator>
	uchar21_t dec_codepoint(BidirectionalIterator &cur, const BidirectionalIterator begin) const
	{
		uchar21_t codepoint = 0;
		unsigned int shift = 0;

		for (std::size_t i = 0; i < maxseqlen && cur != begin; ++i)
		{
			--cur;
			if ((*cur & 0xc0) != 0x80)
			{
				if (i == 0)
					codepoint = static_cast<uchar21_t>(codepoint | ((*cur & 0x7f) << shift));	//  1 octet.
				else if (i == 1)
					codepoint = static_cast<uchar21_t>(codepoint | ((*cur & 0x1f) << shift));	//  2 octets.
				else if (i == 2)
					codepoint = static_cast<uchar21_t>(codepoint | ((*cur & 0x0f) << shift));	//  3 octets.
				else	//  if (i == 3)
					codepoint = static_cast<uchar21_t>(codepoint | ((*cur & 0x07) << shift));	//  4 octets.

				break;
			}
			codepoint = static_cast<uchar21_t>(codepoint | ((*cur & 0x3f) << shift));
			shift += 6;
		}
		return codepoint;
	}

#endif	//  SRELL_NO_REVERSE
};

template <class charT>
struct u16regex_traits : public regex_internal::traits_core<charT>
{
private:

	typedef regex_internal::traits_core<charT> base_type;

public:

	typedef regex_internal::uchar21_t uchar21_t;
	typedef typename base_type::char_type char_type;
	typedef typename base_type::string_type string_type;
	typedef typename base_type::locale_type locale_type;
	typedef typename base_type::char_class_type char_class_type;

	//  utf-16 specific.
	static const std::size_t maxseqlen = 2;

	template <typename InputIterator>
	uchar21_t codepoint(InputIterator begin, const InputIterator end) const
	{
		return codepoint_inc(begin, end);
	}

	template <typename InputIterator>
	uchar21_t codepoint_inc(InputIterator &begin, const InputIterator end) const
	{
		uchar21_t codepoint = static_cast<uchar21_t>(*begin++);

		if ((codepoint & 0xdc00) == 0xd800 && begin != end && (*begin & 0xdc00) == 0xdc00)
			return static_cast<uchar21_t>((((codepoint & 0x3ff) << 10) | (*begin++ & 0x3ff)) + 0x10000);

		return static_cast<uchar21_t>(codepoint & 0xffff);
	}

	template <typename BidirectionalIterator>
	uchar21_t prevcodepoint(BidirectionalIterator cur) const
	{

		uchar21_t codepoint = static_cast<uchar21_t>(*--cur);

		if ((codepoint & 0xdc00) == 0xdc00 && (*--cur & 0xdc00) == 0xd800)
			return static_cast<uchar21_t>((((*cur & 0x3ff) << 10) | (codepoint & 0x3ff)) + 0x10000);

		return static_cast<uchar21_t>(codepoint & 0xffff);
	}

#if !defined(SRELL_NO_REVERSE)

	template <typename BidirectionalIterator>
	uchar21_t dec_codepoint(BidirectionalIterator &cur, const BidirectionalIterator begin) const
	{
		uchar21_t codepoint = static_cast<uchar21_t>(*--cur);

		if ((codepoint & 0xdc00) != 0xdc00 || cur == begin)
			;	//  do nothing.
		else if ((*--cur & 0xdc00) == 0xd800)
			return static_cast<uchar21_t>((((*cur & 0x3ff) << 10) | (codepoint & 0x3ff)) + 0x10000);
		else
			++cur;

		return static_cast<uchar21_t>(codepoint & 0xffff);
	}

#endif	//  SRELL_NO_REVERSE
};

//  specialisation for char.
template <>
struct regex_traits<char> : public regex_internal::traits_core<char>
{
private:

	typedef regex_internal::traits_core<char> base_type;

public:

	typedef regex_internal::uchar21_t uchar21_t;
	typedef base_type::char_type char_type;
	typedef base_type::string_type string_type;
	typedef base_type::locale_type locale_type;
	typedef base_type::char_class_type char_class_type;

	template <typename InputIterator>
	uchar21_t codepoint(InputIterator begin, const InputIterator /* end */) const
	{
		return static_cast<uchar21_t>(static_cast<unsigned char>(*begin));
	}

	template <typename InputIterator>
	uchar21_t codepoint_inc(InputIterator &begin, const InputIterator /* end */) const
	{
		return static_cast<uchar21_t>(static_cast<unsigned char>(*begin++));
	}

	template <typename BidirectionalIterator>
	uchar21_t prevcodepoint(BidirectionalIterator cur) const
	{
		return static_cast<uchar21_t>(static_cast<unsigned char>(*--cur));
	}

#if !defined(SRELL_NO_REVERSE)

	template <typename BidirectionalIterator>
	uchar21_t dec_codepoint(BidirectionalIterator &cur, const BidirectionalIterator /* begin */) const
	{
		return static_cast<uchar21_t>(static_cast<unsigned char>(*--cur));
	}

#endif	//  SRELL_NO_REVERSE
};

//  specialisation for signed char.
template <>
struct regex_traits<signed char> : public regex_traits<char>
{
};

//  (signed) short, (signed) int, (signed) long, (signed) long long, ...

#if defined(SRELL_CPP11_CHAR1632_ENABLED)
template <>
struct regex_traits<char16_t> : public u16regex_traits<char16_t>
{
};
#endif

//  ... "regex_traits.hpp"]
//  ["rei_char_class.hpp" ...

	namespace regex_internal
	{

struct range_pair	//  , public std::pair<charT, charT>
{
	uchar21_t first;
	uchar21_t second;

	range_pair()
	{
	}

	range_pair(const uchar21_t &min, const uchar21_t &max)
		: first(min), second(max)
	{
	}

	range_pair(const uchar21_t &minmax) : first(minmax), second(minmax)
	{
	}

#if 0
	range_pair(const range_pair &right)
		: first(right.first), second(right.second)
	{
	}
#endif

	bool is_range_valid() const
	{
		return first <= second;
	}

	bool is_included(const uchar21_t &c) const
	{
		return first <= c && c <= second;
	}

	bool operator==(const range_pair &right) const
	{
		return this->first == right.first && this->second == right.second;
	}

	bool operator<(const range_pair &right) const
	{
		assert(is_range_valid());
		assert(right.is_range_valid());

		return this->second < right.first;
	}

	void swap(range_pair &right)
	{
		const range_pair tmp = *this;
		*this = right;
		right = tmp;
	}
};

struct range_pairs : public simple_array<range_pair>
{
	typedef range_pair code_range;
	typedef simple_array<code_range> base_type;
	typedef base_type::size_type size_type;

	bool is_included(const uchar21_t &ch) const
	{
		for (size_type i = 0; i < this->size(); ++i)
		{
			if ((*this)[i].is_included(ch))
				return true;
		}
		return false;
	}

	void optimise()
	{
		size_type curno = this->size();

		if (curno)
		{
			while (--curno)
			{
				size_type prevno = curno;

				do
				{
					--prevno;

					if (unify_range((*this)[prevno], (*this)[curno]))
					{
						this->erase(curno);
						break;
					}
				}
				while (prevno);
			}
		}
		sort();
	}

	void negation()
	{
		uchar21_t begin = 0;
		uchar21_t end = 0;
		range_pairs newpairs;
		code_range newrange;

		sort();
		for (size_type i = 0; i < this->size(); ++i)
		{
			const code_range &range = (*this)[i];

			end = range.first;
			if (begin < end)
			{
				newrange.first = begin;
				newrange.second = end - 1;
				newpairs.push_back(newrange);
			}
			begin = range.second + 1;
		}
		if (begin <= constants::unicode_max_codepoint)
		{
			newrange.first = begin;
			newrange.second = constants::unicode_max_codepoint;
			newpairs.push_back(newrange);
		}
		*this = newpairs;

	}

	bool is_overlap(const range_pairs &right) const
	{
		for (size_type i = 0; i < this->size(); ++i)
		{
			const code_range &leftrange = (*this)[i];

			for (size_type j = 0; j < right.size(); ++j)
			{
				const code_range &rightrange = right[j];

				if (rightrange.first <= leftrange.second)	//  Excludes l1 l2 < r1 r2.
					if (leftrange.first <= rightrange.second)	//  Excludes r1 r2 < l1 l2.
						return true;
			}
		}
		return false;
	}

	bool operator==(const range_pairs &right) const
	{
		if (this->size() == right.size())
		{
			for (size_type i = 0; i < this->size(); ++i)
			{
				const code_range &leftrange = (*this)[i];
				bool found = false;

				for (size_type j = 0; j < right.size(); ++j)
				{
					const code_range &rightrange = right[j];

					if (leftrange == rightrange)
					{
						found = true;
						break;
					}
				}
				if (!found)
					return false;
			}
			return true;
		}
		return false;
	}

private:

	bool unify_range(code_range &left, const code_range &right) const
	{
		const uchar21_t left_second_plus1  = left.second + 1;
		const uchar21_t right_second_plus1 = right.second + 1;

		if (right.first <= left_second_plus1 || left_second_plus1 == 0)	//  r1 <= l2+1
		{
			//  l1 l2+1 < r1 r2 excluded.

			if (left.first <= right_second_plus1 || right_second_plus1 == 0)	//  l1 <= r2+1
			{
				//  r1 r2+1 < l1 l2 excluded.

				if (left.first > right.first)
					left.first = right.first;

				if (left.second < right.second)
					left.second = right.second;

				return true;
			}
		}
		return false;
	}

	void sort()
	{
		size_type end = this->size();

		while (end)
		{
			--end;

			for (size_type i = 0; i < end; ++i)
			{
				if ((*this)[i + 1] < (*this)[i])
					(*this)[i + 1].swap((*this)[i]);
			}
		}
	}

public:	//  for debug.

	void print_pairs(const bool, const char *const = NULL, const char *const = NULL) const;
};
//  range_pairs

class re_character_class
{
public:

	typedef range_pairs code_range_array;
	typedef code_range_array::code_range code_range;

	static const unsigned int newline = 0;
	static const unsigned int space   = 1;
	static const unsigned int digit   = 2;
	static const unsigned int word    = 3;
	static const unsigned int number_of_predefcls = 4;

private:

	typedef std::vector<code_range_array> char_class_array;
	typedef char_class_array::size_type char_class_size;

public:

	re_character_class()
	{
		setup_predefinedclass();
	}

	re_character_class &operator=(const re_character_class &that)
	{
		this->char_class_ = that.char_class_;
		return *this;
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	re_character_class &operator=(re_character_class &&that)
	{
		this->char_class_ = std::move(that.char_class_);
		return *this;
	}
#endif

//	bool is_included(const char_class_size &class_number, const charT &c) const
	bool is_included(const unsigned int &class_number, const uchar21_t &c) const
	{
		return char_class_[class_number].is_included(c);
	}

	bool is_newline(const uchar21_t &c) const
	{
		return char_class_[newline].is_included(c);
	}

	bool is_space(const uchar21_t &c) const
	{
		return char_class_[space].is_included(c);
	}

	bool is_word(const uchar21_t &c) const
	{
		return char_class_[word].is_included(c);
	}

	void clear()
	{
		char_class_.resize(number_of_predefcls);
//		setup_predefinedclass();
	}

	void push_back(const code_range_array &cra)
	{
		char_class_.push_back(cra);
	}

	const code_range_array &operator[](const char_class_size no) const
	{
		return char_class_[no];
	}

	unsigned int nextclassno() const
	{
		return static_cast<unsigned int>(char_class_.size());
	}

private:

	void setup_predefinedclass()
	{
		code_range_array ranges;

		//  newline
		ranges.clear();
		ranges.push_back(range_pair(0x000a));
		ranges.push_back(range_pair(0x000d));
		ranges.push_back(range_pair(0x2028, 0x2029));
		char_class_.push_back(ranges);

		//  space
		ranges.clear();
		ranges.push_back(range_pair(0x0020));	//  ' '
		ranges.push_back(range_pair(0x0009, 0x000d));	//  \t \n \v \f \r
		ranges.push_back(range_pair(0x00a0));	//  <NBSP>
		ranges.push_back(range_pair(0x3000));	//  "Zs"
		ranges.push_back(range_pair(0x2000, 0x200a));
		ranges.push_back(range_pair(0x2028, 0x2029));
		ranges.push_back(range_pair(0x202f));
		ranges.push_back(range_pair(0x205f));
		ranges.push_back(range_pair(0x1680));
		ranges.push_back(range_pair(0x180e));
		ranges.push_back(range_pair(0xfeff));	//  <BOM>
		char_class_.push_back(ranges);

		//  digit
		ranges.clear();
		ranges.push_back(range_pair(0x30, 0x39));	//  '0'-'9'
		char_class_.push_back(ranges);

		//  word
		//  inherits 0030..0039 in ranges.
		ranges.push_back(range_pair(0x41, 0x5a));	//  'A'-'Z'
		ranges.push_back(range_pair(0x61, 0x7a));	//  'a'-'z'
		ranges.push_back(range_pair(0x5f));	//  '_'
		char_class_.push_back(ranges);
	}

private:

	char_class_array char_class_;

public:	//  for debug.

	void print_classes(const bool) const;
};
//  re_character_class

	}	//  namespace regex_internal

//  ... "rei_char_class.hpp"]
//  ["rei_state.hpp" ...

	namespace regex_internal
	{

struct re_quantifier
{
	static const unsigned int infinity = static_cast<unsigned int>(~0);

		//  (Special Case 1) in roundbracket_open atleast and atmost represent the
		//    minimum and maximum bracket numbers respectively inside the brackets itself.
	unsigned int atleast;
		//  (Special Case 2) in lookaround_open represents the number of characters to be rewound.
	unsigned int atmost;
	bool is_greedy;

	void reset(const unsigned int len = 1)
	{
		atleast = atmost = len;
		is_greedy = true;
	}

	bool is_valid() const
	{
		return atleast <= atmost && atmost > 0;
	}

	void set_infinity()
	{
		atmost = infinity;
	}

	bool is_infinity() const
	{
		return atmost == infinity;
	}

	bool is_same() const
	{
		return atleast == atmost;
	}

	bool is_default() const
	{
		return atleast == 1 && atmost == 1;
	}
};
//  re_quantifier

//template <typename charT>
struct re_state
{
	union
	{
//		charT character;
		uchar21_t character;	//  for character.
		unsigned int number;	//  for character_class, brackets, counter, repeat, backreference.
//		typename regex_traits<charT>::char_class_type char_class;
	};

	re_state_type type;

	union
	{
		ptrdiff_t next1;
		re_state *next_state1;
		//  points to the next state.
		//  (Special Case 1) in lookaround_open points to the next of lookaround_close.
		//  (Special Case 2) in lookaround_pop points to the content of brackets instead of lookaround_open.
	};
	union
	{
		ptrdiff_t next2;
		re_state *next_state2;
		//  character and character_class: points to another possibility, non-backtracking.
		//  epsilon: points to another possibility, backtracking.
		//  increment_counter, save_and_reset_counter, roundbracket_open, repeat_in_push,
		//    and lookaround_open: points to a restore state, backtracking.
		//  check_counter: complementary to next1 based on quantifier.is_greedy.
		//  (Special Case) roundbracket_close, check_0_width_repeat, and backreference:
		//    points to the next state as an exit after 0 width match.
	};

	re_quantifier quantifier;	//  for check_counter and (?<=...) and (?<!...).

	union
	{
		bool is_not;	//  for character_class, \B, (?!...) and (?<!...).
//		bool multiline;	//  for ^ and $.
		bool dont_push;	//  for check_counter, save_and_reset_counter, roundbracket_open and roundbracket_pop.
	};

#if 0
	union
	{
		bool icase;		//  for character, character_class and backreference.
		bool reverse;	//  for (?<=...) and (?<!...).
	};
#endif

	//  st_character,               //  0x00
		//  char/number:        character
		//  next1:              gen.
		//  next2:              1 (exit. used only when '*')
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_character_class,         //  0x01
		//  char/number:        character-class-number
		//  next1:              gen.
		//  next2:              1 (exit. used only when '*')
		//  quantifiers:        -
		//  is_not/dont_push:   not

	//  st_epsilon,                 //  0x02
		//  char/number:        -
		//  next1:              gen.
		//  next2:              alt.
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_check_counter,           //  0x03
		//  char/number:        counter-number
		//  next1:              greedy: increment_counter or repeat_in_push
		//                      not-greedy: out-of-loop
		//  next2:              complementary to next1
		//  q.atleast:          gen.
		//  q.atmost:           gen.
		//  q.greedy:           gen.
		//  is_not/dont_push:   dont_push

	//  st_increment_counter,       //  0x04
		//  char/number:        counter-number
		//  next1:              2 (next of decrement_counter, atom)
		//  next2:              1 (decrement_counter)
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_decrement_counter,       //  0x05
		//  char/number:        counter-number
		//  next1:              - (always treated as 'unmatched')
		//  next2:              -
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_save_and_reset_counter,  //  0x06
		//  char/number:        counter-number
		//  next1:              2 (check_counter)
		//  next2:              1 (restore_counter)
		//  quantifiers:        -
		//  is_not/dont_push:   dont_push

	//  st_restore_counter,         //  0x07
		//  char/number:        counter-number
		//  next1:              - (always treated as 'unmatched')
		//  next2:              -
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_roundbracket_open,       //  0x08
		//  char/number:        bracket-number
		//  next1:              2 (next of roundbracket_pop, atom)
		//  next2:              1 (roundbracket_pop)
		//  q.atleast:          min bracket number inside this bracket (i.e., myself number)
		//  q.atmost:           max bracket number inside this bracket
		//  q.greedy:           -
		//  is_not/dont_push:   dont_push

	//  st_roundbracket_pop,        //  0x09
		//  char/number:        bracket-number
		//  next1:              - (always treated as 'unmatched')
		//  next2:              -
		//  q.atleast:          min bracket number inside this bracket (i.e., myself number)
		//  q.atmost:           max bracket number inside this bracket
		//  q.greedy:           -
		//  is_not/dont_push:   dont_push

	//  st_roundbracket_close,      //  0x0a
		//  char/number:        bracket-number
		//  next1:              gen. (or the previous of roundbracket_open if a
		//                            bracket has a quantifier)
		//  next2:              1 (exit for 0-width loop)
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_repeat_in_push,          //  0x0b
		//  char/number:        repeat-counter
		//  next1:              2 (check_0_width_repeat)
		//  next2:              1 (repeat_in_pop)
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_repeat_in_pop,           //  0x0c
		//  char/number:        repeat-counter
		//  next1:              - (always treated as 'unmatched')
		//  next2:              -
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_check_0_width_repeat,    //  0x0d
		//  char/number:        repeat-counter
		//  next1:              gen. (epsilon or check_counter)
		//  next2:              1 (exit for 0-width loop)
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_backreference,           //  0x0e
		//  char/number:        bracket-number
		//  next1:              gen.
		//  next2:              1 (exit for 0-width match)
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_lookaround_open,         //  0x0f
		//  char/number:        -
		//  next1:              next of lookaround_close (to where jumps after look-around)
		//  next2:              1 (lookaround_pop)
		//  q.atleast:          number of chars to be rewound (for (?<=...) (?<!...))
		//  q.atmost:           -
		//  q.greedy:           -
		//  is_not/dont_push:   not

	//  st_lookaround_pop,          //  0x10
		//  char/number:        -
		//  next1:              1 (used by lookaround_open. this type is treated
		//                         as 'unmatched' and does not use this value)
		//  next2:              -
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_bol,                     //  0x11
		//  char/number:        -
		//  next1/next2:        -
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_eol,                     //  0x12
		//  char/number:        -
		//  next1/next2:        -
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_boundary,                //  0x13
		//  char/number:        -
		//  next1/next2:        -
		//  quantifiers:        -
		//  is_not/dont_push:   not

	//  st_success,                 //  0x14
		//  char/number:        -
		//  next1/next2:        -
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_move_nextpos,            //  0x15
		//  char/number:        -
		//  next1/next2:        -
		//  quantifiers:        -
		//  is_not/dont_push:   -

	void reset()
	{
		number = 0;
		type   = st_character;
		next1  = 1;
		next2  = 0;
		is_not = false;
//		icase = false;
		quantifier.reset();
	}
};
//  re_state

struct re_flags
{
	bool i;
//	bool m;

#if defined(SRELLTEST_PUSHPOP_OPT)
	unsigned int depth;
#endif

	void reset(const regex_constants::syntax_option_type &flags)
	{
		i = (flags & regex_constants::icase) != 0;	//  case-insensitive.
//		m = (flags & regex_constants::multiline) != 0;

#if defined(SRELLTEST_PUSHPOP_OPT)
		depth = 0;
#endif
	}
};
//  re_flags

	}	//  namespace regex_internal

//  ... "rei_state.hpp"]
//  ["rei_compiler.hpp" ...

	namespace regex_internal
	{

template <typename charT, typename traits>
struct re_object_core
{
protected:

	typedef re_state/*<charT>*/ state_type;
	typedef re_character_class char_class_type;
	typedef typename char_class_type::code_range_array code_range_array;
	typedef simple_array<state_type> state_array;

	regex_constants::syntax_option_type flags_;
	state_array NFA_states;
	char_class_type character_class;

#if !defined(SRELLDBG_NO_1STCHRCLS)
  #if !defined(SRELLDBG_NO_BITSET)
	std::bitset<constants::unicode_max_codepoint + 1> firstchar_class_bs;	//  136 KiB.
  #else
	code_range_array firstchar_class;
  #endif
#endif

#if !defined(SRELL_NO_LIMIT_COUNTER)
public:

	std::size_t limit_counter;

protected:
#endif

	traits traits_inst;

	unsigned int number_of_brackets;
	unsigned int number_of_counters;
	unsigned int number_of_repeats;

#if !defined(SRELL_NO_LIMIT_COUNTER)
private:

	static const std::size_t lcounter_defnum_ = 16777216;

#endif

protected:

	re_object_core()
#if !defined(SRELL_NO_LIMIT_COUNTER)
		: limit_counter(lcounter_defnum_)
#endif
	{
	}

	re_object_core(const re_object_core &right)
	{
		operator=(right);
	}

	void reset()
	{
		flags_ = 0;
		NFA_states.clear();
		character_class.clear();

#if !defined(SRELLDBG_NO_1STCHRCLS)
  #if !defined(SRELLDBG_NO_BITSET)
  		firstchar_class_bs.reset();
  #else
		firstchar_class.clear();
  #endif
#endif

#if !defined(SRELL_NO_LIMIT_COUNTER)
		limit_counter = lcounter_defnum_;
#endif

		number_of_brackets = 1;
		number_of_counters = 0;
		number_of_repeats  = 0;
	}

#if !defined(SRELLDBG_DISABLE_COPY)
	re_object_core &operator=(const re_object_core &that)
	{
		if (this != &that)
		{
			this->flags_ = that.flags_;
			this->NFA_states = that.NFA_states;
			this->character_class = that.character_class;

#if !defined(SRELLDBG_NO_1STCHRCLS)
  #if !defined(SRELLDBG_NO_BITSET)
			this->firstchar_class_bs = that.firstchar_class_bs;
  #else
			this->firstchar_class = that.firstchar_class;
  #endif
#endif

#if !defined(SRELL_NO_LIMIT_COUNTER)
			this->limit_counter = that.limit_counter;
#endif

			this->traits_inst = that.traits_inst;

			this->number_of_brackets = that.number_of_brackets;
			this->number_of_counters = that.number_of_counters;
			this->number_of_repeats = that.number_of_repeats;

			if (that.NFA_states.size())
				repair_nextstates(&that.NFA_states[0]);
		}
		return *this;
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	re_object_core &operator=(re_object_core &&that)
	{
		if (this != &that)
		{
			this->flags_ = that.flags_;
			this->NFA_states = std::move(that.NFA_states);
			this->character_class = std::move(that.character_class);

#if !defined(SRELLDBG_NO_1STCHRCLS)
  #if !defined(SRELLDBG_NO_BITSET)
			this->firstchar_class_bs = std::move(that.firstchar_class_bs);
  #else
			this->firstchar_class = std::move(that.firstchar_class);
  #endif
#endif

#if !defined(SRELL_NO_LIMIT_COUNTER)
			this->limit_counter = that.limit_counter;
#endif

//			this->traits_inst = std::move(that.traits_inst);
			static_cast<regex_internal::traits_core<charT> &>(this->traits_inst) = std::move(that.traits_inst);
				//  call explicitly traits_core<charT>::operator=(&&); otherwise
				//  vc++10 calls traits_core<charT>::operator=(const &) unless
				//  regex_traits has both operator=(const &) and operator=(&&) that
				//  merely call traits_core<charT>::operator=()s respectively.

			this->number_of_brackets = that.number_of_brackets;
			this->number_of_counters = that.number_of_counters;
			this->number_of_repeats = that.number_of_repeats;
		}
		return *this;
	}

#endif	//  SRELL_CPP11_MOVE_ENABLED
#endif	//  SRELLDBG_DISABLE_COPY

	void throw_error(const regex_constants::error_type &e)
	{
//		reset();
		NFA_states.clear();
		throw regex_error(e);
	}

private:

	void repair_nextstates(const state_type *const oldbase)
	{
		state_type *const newbase = &this->NFA_states[0];
		const typename state_array::size_type size = this->NFA_states.size();

		for (typename state_array::size_type i = 0; i < size; ++i)
		{
			state_type &state = this->NFA_states[i];

			if (state.next_state1)
				state.next_state1 = state.next_state1 - oldbase + newbase;

			if (state.next_state2)
				state.next_state2 = state.next_state2 - oldbase + newbase;
		}
	}
};
//  re_object_core

template <typename charT, typename traits>
class re_compiler : public re_object_core<charT, traits>
{
private:

	typedef re_object_core<charT, traits> base_type;

	typedef typename base_type::state_type state_type;
	typedef typename base_type::state_array state_array;
	typedef typename base_type::char_class_type char_class_type;
	typedef typename base_type::code_range_array code_range_array;
	typedef typename char_class_type::code_range code_range_type;

protected:

	template <typename InputIterator>
	bool compile(InputIterator first, const InputIterator &last, const regex_constants::syntax_option_type flags = regex_constants::ECMAScript)
	{
		re_quantifier piececharlen;
		re_flags compile_flags;
		state_type atom;

		this->reset();
		this->flags_ = flags;
		compile_flags.reset(flags);

		atom.reset();
		atom.type = st_epsilon;
		this->NFA_states.push_back(atom);

		if (!make_nfa_states(this->NFA_states, piececharlen, first, last, compile_flags))
		{
#ifdef _DEBUG
			::fprintf(stdout, "Failed near %c\n", *first);
#endif
			return false;
		}

		if (first != last)
			this->throw_error(regex_constants::error_paren);	//  ')'s are too many.

		if (!check_backreference_wrapped_in_self())
			this->throw_error(regex_constants::error_backref);

		atom.type = st_success;
		atom.next1 = 0;
		this->NFA_states.push_back(atom);

		optimise();
		relativejump_to_absolutejump();

		return true;
	}

	bool is_icase() const
	{
		if (this->flags_ & regex_constants::icase)
			return true;
		return false;
	}

	bool is_multiline() const
	{
		if (this->flags_ & regex_constants::multiline)
			return true;
		return false;
	}

private:

	template <typename InputIterator>
	bool make_nfa_states(state_array &NFA_states, re_quantifier &piececharlen, InputIterator &curpos, const InputIterator &last, re_flags &current_flags)
	{
		typename state_array::size_type prevbranch_end = 0;
		state_type atom;
		state_array branch;
		re_quantifier branchsize;

		piececharlen.reset(0);

		for (;;)
		{
			branch.clear();

			if (!make_branch(branch, branchsize, curpos, last, current_flags))
				return false;

			//  For piececharlen.atleast, 0 as the initial value and 0 as an
			//  actual value must be distinguished.
			if (!piececharlen.atmost || piececharlen.atleast > branchsize.atleast)
				piececharlen.atleast = branchsize.atleast;

			if (piececharlen.atmost < branchsize.atmost)
				piececharlen.atmost = branchsize.atmost;

			if (curpos != last && static_cast<uchar21_t>(*curpos) == meta_char::mc_bar)
			{
				atom.reset();
				atom.type = st_epsilon;
				atom.next2 = static_cast<ptrdiff_t>(branch.size()) + 2;
				branch.insert(0, atom);
			}

			if (prevbranch_end)
				NFA_states[prevbranch_end].next1 = static_cast<ptrdiff_t>(branch.size()) + 1;

			NFA_states += branch;

				//  last or ')'
			if (curpos == last || static_cast<uchar21_t>(*curpos) == meta_char::mc_rbracl)
				break;

			//  *curpos == '|'

			prevbranch_end = NFA_states.size();
			atom.reset();
			atom.type = st_epsilon;
			NFA_states.push_back(atom);

			++curpos;
		}
		return true;
	}

	template <typename InputIterator>
	bool make_branch(state_array &branch, re_quantifier &branchsize, InputIterator &curpos, const InputIterator &last, re_flags &current_flags)
	{
		state_array piece;
		state_array piece_with_quantifier;
		re_quantifier quantifier;

		branchsize.reset(0);

		for (;;)
		{
			re_quantifier piececharlen;

			if (curpos == last)
				return true;

			piece.clear();
			piece_with_quantifier.clear();

			switch (*curpos)
			{
//			case char_ctrl::cc_nul:	//  '\0':
			case meta_char::mc_bar:	//  '|':
			case meta_char::mc_rbracl:	//  ')':
				return true;

			default:
				if (!get_atom(piece, piececharlen, curpos, last, current_flags))
					return false;
			}

			if (piece.size())
			{
				const state_type &firstatom = piece[0];
				quantifier.reset();	//  quantifier.atleast = quantifier.atmost = 1;

				//  1. character:  size == 1 && type == character,
				//  2. [...]:      size == 1 && type == character_class,
				//  3. (...):      size == ? && type == roundbracket_open,
				//  4. (?:...):    size == ? && type == epsilon && character == ':',
				//  5. backref:    size == 1 && type == backreference,
				//  -- assertions boundary --
				//  6. lookaround: size == ? && type == lookaround,
				//  7. assertion:  size == 1 && type == one of assertions (^, $, \b and \B).
#if !defined(SRELL_ENABLE_GT)
				if (firstatom.type < st_zero_width_boundary)
#else
				//  5.5. independent: size == ? && type == lookaround && character == '>',
				if (firstatom.type < st_zero_width_boundary || (firstatom.type == st_lookaround_open && firstatom.character == meta_char::mc_gt))
#endif
				{
					if (curpos != last && !get_quantifier(quantifier, curpos, last))
						return false;
				}

#if defined(SRELLTEST_PUSHPOP_OPT)
				combine_piece_with_quantifier(piece_with_quantifier, piece, quantifier, piececharlen, current_flags);
#else
				combine_piece_with_quantifier(piece_with_quantifier, piece, quantifier, piececharlen);
#endif

				branchsize.atleast += piececharlen.atleast * quantifier.atleast;
				if (!branchsize.is_infinity())
				{
					if (piececharlen.is_infinity() || quantifier.is_infinity())
						branchsize.set_infinity();
					else
						branchsize.atmost += piececharlen.atmost * quantifier.atmost;
				}
				branch += piece_with_quantifier;
			}
		}
	}

	template <typename InputIterator>
	bool get_atom(state_array &piece, re_quantifier &atomsize, InputIterator &curpos, const InputIterator &last, re_flags &current_flags)
	{
		state_type atom;

		atom.reset();
		atom.character = this->traits_inst.codepoint_inc(curpos, last);

		switch (atom.character)
		{
		case meta_char::mc_rbraop:	//  '(':
			return get_piece_in_roundbrackets(piece, atomsize, curpos, last, current_flags);

		case meta_char::mc_sbraop:	//  '[':
			if (!register_character_class(atom, curpos, last, current_flags))
				return false;

			break;

		case meta_char::mc_escape:	//  '\\':
			if (!translate_atom_escape(atom, curpos, last))
				return false;

			break;

		case meta_char::mc_period:	//  '.':
			atom.type = st_character_class;
			atom.number = re_character_class::newline;
			atom.is_not = true;
			break;

		case meta_char::mc_caret:	//  '^':
			atom.type = st_bol;
			atom.quantifier.reset(0);
//			if (current_flags.m)
//				atom.multiline = true;
			break;

		case meta_char::mc_dollar:	//  '$':
			atom.type = st_eol;
			atom.quantifier.reset(0);
//			if (current_flags.m)
//				atom.multiline = true;
			break;

		case meta_char::mc_astrsk:	//  '*':
		case meta_char::mc_plus:	//  '+':
		case meta_char::mc_query:	//  '?':
		case meta_char::mc_cbraop:	//  '{'
			this->throw_error(regex_constants::error_badrepeat);

		default:;
		}

		if (atom.type == st_character)
		{
			if (current_flags.i)
				atom.character = this->traits_inst.unicode_casefolding(atom.character);
		}

		piece.push_back(atom);
		atomsize = atom.quantifier;

		return true;
	}

	//  '('.

	template <typename InputIterator>
	bool get_piece_in_roundbrackets(state_array &piece, re_quantifier &piececharlen, InputIterator &curpos, const InputIterator &last, re_flags &current_flags)
	{
		const re_flags original_flags(current_flags);
		state_type atom;

		if (curpos == last)
			this->throw_error(regex_constants::error_paren);

		atom.reset();

		if (static_cast<uchar21_t>(*curpos) == meta_char::mc_query)	//  '?'
		{
			if (!extended_roundbrackets(piece, atom, ++curpos, last))
				return false;
		}
		else
		{
			push_bracket_open(piece, atom);
		}

//		if (curpos == last)
//			this->throw_error(regex_constants::error_paren);

#if defined(SRELLTEST_PUSHPOP_OPT)
		++current_flags.depth;
#endif

		if (!make_nfa_states(piece, piececharlen, curpos, last, current_flags))
			return false;

#if defined(SRELLTEST_PUSHPOP_OPT)
//		--current_flags.depth;
		//  Done automatically through current_flags = original_flags;
#endif

		//  last or ')'?
		if (curpos == last)
			this->throw_error(regex_constants::error_paren);

		++curpos;

		current_flags = original_flags;

		switch (atom.type)
		{
		case st_epsilon:
			assert(piece[0].character == meta_char::mc_colon);

			if (piece.size() <= 2)	//  ':' or ':' + one.
			{
				piece.erase(0);
				return true;
			}
			break;

		case st_lookaround_pop:

			{
				state_type &first_atom = piece[0];

				assert(first_atom.type == st_lookaround_open);

#if !defined(SRELL_NO_REVERSE)
//				if (first_atom.reverse)
				if (first_atom.quantifier.atleast)	//  Marked as lookbehind.
				{
					if (!piececharlen.is_same() || piececharlen.is_infinity())
						this->throw_error(regex_constants::error_lookbehind);

					first_atom.quantifier = piececharlen;
				}
#endif

#if defined(SRELL_ENABLE_GT)
				if (first_atom.character != meta_char::mc_gt)
#endif
					piececharlen.reset(0);

				first_atom.next1 = static_cast<ptrdiff_t>(piece.size()) + 1;

				atom.type  = st_lookaround_close;
				atom.next1 = 0;
			}
			break;

		default:
			set_bracket_close(piece, atom);
		}

		piece.push_back(atom);
		return true;
	}

	template <typename InputIterator>
	bool extended_roundbrackets(state_array &piece, state_type &atom, InputIterator &curpos, const InputIterator last)
	{
#if !defined(SRELL_NO_REVERSE)
//		bool back = false;
#endif

		if (curpos == last)
			this->throw_error(regex_constants::error_paren);

#if !defined(SRELL_NO_REVERSE)
		if (static_cast<uchar21_t>(*curpos) == meta_char::mc_lt)	//  '<'
		{
//			back = true;
			++curpos;

			if (curpos == last)
				this->throw_error(regex_constants::error_paren);

			const uchar21_t prefetch = static_cast<uchar21_t>(*curpos);

			if (prefetch != meta_char::mc_eq && prefetch != meta_char::mc_exclam)
				this->throw_error(regex_constants::error_lookbehind);
		}
		else
			atom.quantifier.reset(0);
#endif	//  SRELL_NO_REVERSE

		atom.character = static_cast<uchar21_t>(*curpos);

		switch (*curpos)
		{
		case meta_char::mc_colon:
			atom.type = st_epsilon;
			break;
//			++curpos;
//			return true;

		case meta_char::mc_exclam:	//  '!':
			atom.is_not = true;

		case meta_char::mc_eq:	//  '=':
#if !defined(SRELL_NO_REVERSE)
//			atom.reverse = back;
#endif
#if defined(SRELL_ENABLE_GT)
		case meta_char::mc_gt:
#endif

			atom.type = st_lookaround_open;
			atom.next2 = 1;
			piece.push_back(atom);
			atom.next1 = 1;
			atom.next2 = 0;
			atom.type = st_lookaround_pop;
			break;

		default:
			this->throw_error(regex_constants::error_paren);
		}

		++curpos;
		piece.push_back(atom);
		return true;
	}

	void push_bracket_open(state_array &piece, state_type &atom)
	{
		atom.type   = st_roundbracket_open;	//  '('
		atom.number = this->number_of_brackets;
		atom.next1  = 2;
		atom.next2  = 1;
		piece.push_back(atom);
		++this->number_of_brackets;

		atom.type  = st_roundbracket_pop;
		atom.next1 = 0;
		atom.next2 = 0;
		piece.push_back(atom);
	}

	void set_bracket_close(state_array &piece, state_type &atom)
	{
		unsigned int max_bracketno = atom.number;

		atom.type = st_roundbracket_close;
		atom.next1 = 1;
		atom.next2 = 1;

		for (typename state_array::size_type i = 0; i < piece.size(); ++i)
		{
			const state_type &state = piece[i];

			if (state.type == st_roundbracket_open && max_bracketno < state.number)
				max_bracketno = state.number;
		}

		assert(piece.size() >= 2);
		re_quantifier &rb_open = piece[0].quantifier;
		re_quantifier &rb_pop = piece[1].quantifier;

		rb_open.atleast = rb_pop.atleast = atom.number;
		rb_open.atmost = rb_pop.atmost = max_bracketno;
	}

#if defined(SRELLTEST_PUSHPOP_OPT)
	void combine_piece_with_quantifier(state_array &piece_with_quantifier, state_array &piece, const re_quantifier &quantifier, const re_quantifier &piececharlen, const re_flags &current_flags)
#else
	void combine_piece_with_quantifier(state_array &piece_with_quantifier, state_array &piece, const re_quantifier &quantifier, const re_quantifier &piececharlen)
#endif
	{
		const bool firstpiece_is_roundbracket_open = (piece[0].type == st_roundbracket_open);
		state_type atom;

		if (quantifier.atmost == 0)
			return;

		atom.reset();
		atom.quantifier = quantifier;

		if (quantifier.atmost == 1)
		{
			if (quantifier.atleast == 0)
			{
				atom.type  = st_epsilon;
				atom.next2 = static_cast<ptrdiff_t>(piece.size()) + 1;

				if (!quantifier.is_greedy)
				{
					atom.next1 = atom.next2;
					atom.next2 = 1;
				}
				piece_with_quantifier.push_back(atom);
				//      (push)
			}
#if defined(SRELLTEST_PUSHPOP_OPT)

#if 01
			if (current_flags.depth == 0)
				make_roundbrackets_and_counters_not_to_push(&piece[0], &piece[0] + piece.size());
#else
			if (current_flags.depth == 0 && firstpiece_is_roundbracket_open)
			{
				piece[0].dont_push = true;	//  st_roundbracket_open.
				piece[1].dont_push = true;	//  st_roundbracket_pop.
			}
#endif

#endif	//  defined(SRELLTEST_PUSHPOP_OPT)
			piece_with_quantifier += piece;
			return;
		}

		//  atmost >= 2

		atom.type = st_epsilon;
		if (quantifier.atleast == 0 && quantifier.is_infinity())	//  {0,}
		{
			//  greedy:  1.epsilon(2|4), 2.piece, 3.LAorC0WR(1|0), 4.OutOfLoop.
			//  !greedy: 1.epsilon(4|2), 2.piece, 3.LAorC0WR(1|0), 4.OutOfLoop.
			//  LAorC0WR: LastAtomOfPiece or Check0WidthRepeat.
			//  atom.type points to 1.
		}
		else if (quantifier.atleast == 1 && quantifier.is_infinity())	//  {1,}
		{
#if !defined(SRELLDBG_NO_ASTERISK_OPT)

			if (piece.size() == 1 && (piece[0].type == st_character || piece[0].type == st_character_class))
			{
				piece_with_quantifier += piece;
				--atom.quantifier.atleast;	//  /.+/ -> /..*/.
			}
			else
#endif
			{
				atom.next1 = 2;
				atom.next2 = 0;
				piece_with_quantifier.push_back(atom);
				//  greedy:  1.epsilon(3), 2.epsilon(3|5), 3.piece, 4.LAorC0WR(2|0), 5.OutOfLoop.
				//  !greedy: 1.epsilon(3), 2.epsilon(5|3), 3.piece, 4.LAorC0WR(2|0), 5.OutOfLoop.
				//  atom.type points to 2.
			}
		}
		else
		{
			atom.number = this->number_of_counters;
			++this->number_of_counters;

			atom.type = st_save_and_reset_counter;

#if defined(SRELLTEST_PUSHPOP_OPT)

			if (current_flags.depth == 0)
			{
				atom.dont_push = true;

				atom.next1 = 1;
				atom.next2 = 0;
			}
			else
#endif	//  defined(SRELLTEST_PUSHPOP_OPT)
			{
				atom.next1 = 2;
				atom.next2 = 1;
				piece_with_quantifier.push_back(atom);

				atom.type  = st_restore_counter;
				atom.next1 = 0;
				atom.next2 = 0;
			}
			piece_with_quantifier.push_back(atom);
			//  1.save_and_reset_counter(3|2), 2.restore_counter(0|0),

			atom.dont_push = false;

			atom.next1 = 0;
			atom.next2 = 0;
			atom.type = st_decrement_counter;
			piece.insert(0, atom);

			atom.next1 = 2;
			atom.next2 = 1;
			atom.type = st_increment_counter;
			piece.insert(0, atom);

			atom.type = st_check_counter;
			//  greedy:  3.check_counter(4|6), 4.piece, 5.LAorC0WR(3|0), 6.OutOfLoop.
			//  !greedy: 3.check_counter(6|4), 4.piece, 5.LAorC0WR(3|0), 6.OutOfLoop.
			//  4.piece = { 4a.increment_counter(4c|4b), 4b.decrement_counter(0|0), 4c.OriginalPiece }.
		}

		//  atom.type is epsilon or check_counter.
		//  Its "next"s point to piece and OutOfLoop.

		if (piececharlen.atleast || firstpiece_is_roundbracket_open)
		{
			const typename state_array::size_type piece_size = piece.size();
			state_type &last_atom = piece[piece_size - 1];

			last_atom.quantifier = atom.quantifier;
			last_atom.next1 = 0 - static_cast<ptrdiff_t>(piece_size);
				//  Points to the one immediately before piece, which will be pushed last in this block.

			//  atom.type has already been set. epsilon or check_counter.
			atom.next1 = 1;
			atom.next2 = static_cast<ptrdiff_t>(piece_size) + 1;
			if (!quantifier.is_greedy)
			{
				atom.next1 = atom.next2;
				atom.next2 = 1;
			}
			piece_with_quantifier.push_back(atom);
		}
		else
		{
			//  atom.type has already been set. epsilon or check_counter.
			atom.next1 = 1;
			atom.next2 = static_cast<ptrdiff_t>(piece.size()) + 4;	//  to OutOfLoop.
				//  The reason for +3 than above is that push, pop, and check_0_width are added below.
			if (!quantifier.is_greedy)
			{
				atom.next1 = atom.next2;
				atom.next2 = 1;
			}
			piece_with_quantifier.push_back(atom);	//  *1

			atom.number = this->number_of_repeats;
			++this->number_of_repeats;

			atom.type  = st_repeat_in_push;	//  '{':
			atom.next1 = 2;
			atom.next2 = 1;
			piece_with_quantifier.push_back(atom);

			atom.type  = st_repeat_in_pop;
			atom.next1 = 0;
			atom.next2 = 0;
			piece_with_quantifier.push_back(atom);

			atom.type  = st_check_0_width_repeat;
			atom.next1 = 0 - static_cast<ptrdiff_t>(piece.size()) - 3;	//  3 for *1, push, and pop.
			atom.next2 = 1;
			piece.push_back(atom);
				//  greedy:  1.epsilon or check_counter(2|6),
				//  !greedy: 1.epsilon or check_counter(6|2),
				//    2.repeat_in_push(4|3), 3.repeat_in_pop(0|0), 4.piece,
				//    5.check_0_width_repeat(1|0), 6.OutOfLoop.
		}
		piece_with_quantifier += piece;
	}

#if defined(SRELLTEST_PUSHPOP_OPT)
	//  If an outer repetition does not exist, no restore operation is required.
	void make_roundbrackets_and_counters_not_to_push(state_type *begin, const state_type *const end)
	{
		while (begin != end)
		{
			state_type &state1 = *begin;

			if (state1.type == st_save_and_reset_counter)
			{
				if ((++begin)->type == st_restore_counter)
				{
					state1.dont_push = true;

					state1.next2 = 0;
					begin->dont_push = true;

					++begin;
				}

				assert(begin->type == st_check_counter);

				begin += (begin->quantifier.is_greedy ? begin->next2 : begin->next1);
				continue;
			}
			if (state1.type == st_roundbracket_open)
			{
				state_type *const after_bracketopen = ++begin;	//  should be st_roundbracket_pop.

//				state1.dont_push = true;

				assert(begin->type == st_roundbracket_pop);
//				begin->dont_push = true;

				for (; begin != end; ++begin)
				{
					if (begin->type == st_roundbracket_close && begin->number == state1.number)
					{
						if (begin->quantifier.atmost <= 1)
						{
							state1.dont_push = true;
							after_bracketopen->dont_push = true;
							make_roundbrackets_and_counters_not_to_push(after_bracketopen, begin);
						}
						++begin;
						break;
					}
				}
			}
			else
				++begin;
		}
	}
#endif	//  defined(SRELLTEST_PUSHPOP_OPT)

	//  '['.

	template <typename InputIterator>
	bool register_character_class(state_type &atom, InputIterator &curpos, const InputIterator &last, const re_flags &current_flags)
	{
		code_range_type code_range;
		code_range_array ranges;
		state_type classatom;

		if (curpos == last)
			this->throw_error(regex_constants::error_brack);

		atom.number = this->character_class.nextclassno();
		atom.type   = st_character_class;

		if (static_cast<uchar21_t>(*curpos) == meta_char::mc_caret)	//  '^'
		{
			atom.is_not = true;
			++curpos;
		}

		for (;;)
		{
			if (curpos == last)
				this->throw_error(regex_constants::error_brack);

			if (static_cast<uchar21_t>(*curpos) == meta_char::mc_sbracl)	//   ']'
				break;

			classatom.reset();

			if (!get_character_in_class(classatom, curpos, last))
				return false;

			if (classatom.type == st_character_class)
			{
				add_predefclass_to_charclass(ranges, classatom.number, classatom.is_not);
				continue;
			}

			code_range.first = code_range.second = classatom.character;

			if (curpos == last)
				this->throw_error(regex_constants::error_brack);

			if (static_cast<uchar21_t>(*curpos) == meta_char::mc_minus)	//  '-'
			{
				++curpos;

				if (curpos == last)
					this->throw_error(regex_constants::error_brack);

				if (static_cast<uchar21_t>(*curpos) == meta_char::mc_sbracl)
				{
					PUSH_SEPARATELY:
					ranges.push_back(code_range);
					code_range.first = code_range.second = meta_char::mc_minus;
				}
				else
				{
					assert(classatom.type == st_character);
					assert(classatom.is_not == false);

					if (!get_character_in_class(classatom, curpos, last))
						return false;

					if (classatom.type == st_character_class)
					{
						add_predefclass_to_charclass(ranges, classatom.number, classatom.is_not);
						goto PUSH_SEPARATELY;
					}

					code_range.second = classatom.character;

					if (!code_range.is_range_valid())
						this->throw_error(regex_constants::error_range);
				}
			}
			ranges.push_back(code_range);
		}

		//  *curpos == ']'
		++curpos;

		if (ranges.size() == 1 && ranges[0].first == ranges[0].second && !atom.is_not)
		{
			atom.type = st_character;
			atom.character = ranges[0].first;
			return true;
		}

		if (current_flags.i)
			make_charcls_nocase(ranges);

		ranges.optimise();

		for (unsigned int i = 0; i < atom.number; ++i)
		{
			if (this->character_class[i] == ranges)
			{
				atom.number = i;
				return true;
			}
		}

		this->character_class.push_back(ranges);

		return true;
	}

	template <typename InputIterator>
	bool get_character_in_class(state_type &atom /* uchar21_t &unichar */, InputIterator &curpos, const InputIterator &last /* , const re_flags &current_flags */)
	{

		atom.character = this->traits_inst.codepoint_inc(curpos, last);

		return atom.character != meta_char::mc_escape	//  '\\'
			|| translate_escseq(atom, curpos, last);
	}

	void add_predefclass_to_charclass(code_range_array &cls, const unsigned int clsno, const bool negation)
	{
		code_range_array predefclass = this->character_class[clsno];

		if (negation)
			predefclass.negation();

		cls += predefclass;
	}

	void make_charcls_nocase(code_range_array &oldpairs)
	{
		code_range_array newpairs;
		code_range_type cfrange;

		for (typename code_range_array::size_type i = 0; i < oldpairs.size(); ++i)
		{
			const code_range_type &range = oldpairs[i];

			for (uchar21_t ch = range.first;; ++ch)
			{
				cfrange.first = cfrange.second = this->traits_inst.unicode_casefolding(ch);
				newpairs.push_back(cfrange);
//				newpairs.optimise();

				if (ch == range.second)
					break;
			}
		}
		oldpairs = newpairs;
	}

#if !defined(SRELLDBG_NO_BITSET)
	void make_caseunfoldedcharset(code_range_array &oldpairs)
	{
		typedef std::basic_string<uchar21_t> u21string;
		code_range_array newpairs;

		for (typename code_range_array::size_type i = 0; i < oldpairs.size(); ++i)
		{
			const code_range_type &range = oldpairs[i];

			for (uchar21_t ch = range.first;; ++ch)
			{
				const u21string table = this->traits_inst.unicode_reverse_casefolding(ch);

				for (typename u21string::size_type i = 0; i < table.size(); ++i)
					newpairs.push_back(code_range_type(table[i]));

//				newpairs.optimise();

				if (ch == range.second)
					break;
			}
		}
		oldpairs = newpairs;
	}
#endif	//  !defined(SRELLDBG_NO_BITSET)

	//  Escape characters which appear both in and out of [] pairs.
	template <typename InputIterator>
	bool translate_escseq(state_type &atom, InputIterator &curpos, const InputIterator &last)
	{
		if (curpos == last)
			this->throw_error(regex_constants::error_escape);

		atom.character = this->traits_inst.codepoint_inc(curpos, last);	//  *curpos++;

		switch (atom.character)
		{
		//  predefined classes.

		case char_alnum::ch_D:	//  'D':
			atom.is_not = true;

		case char_alnum::ch_d:	//  'd':
			atom.number = re_character_class::digit;	//  \d, \D.
			atom.type = st_character_class;
			break;

		case char_alnum::ch_S:	//  'S':
			atom.is_not = true;

		case char_alnum::ch_s:	//  's':
			atom.number = re_character_class::space;	//  \s, \S.
			atom.type = st_character_class;
			break;

		case char_alnum::ch_W:	//  'W':
			atom.is_not = true;

		case char_alnum::ch_w:	//  'w':
			atom.number = re_character_class::word;	//  \w, \W.
			atom.type = st_character_class;
			break;

		//  prepared for Unicode properties and script names.
//		case char_alnum::ch_P:	//  \P{...}
//		case char_alnum::ch_p:	//  \p{...}

#if 0
		case char_alnum::ch_a:
			atom.character = char_ctrl::cc_bell;	//  '\a' 0x07:BEL
			break;
#endif

		case char_alnum::ch_b:
			atom.character = char_ctrl::cc_bs;	//  '\b' 0x08:BS
			break;

		case char_alnum::ch_t:
			atom.character = char_ctrl::cc_htab;	//  '\t' 0x09:HT
			break;

		case char_alnum::ch_n:
			atom.character = char_ctrl::cc_nl;	//  '\n' 0x0a:LF
			break;

		case char_alnum::ch_v:
			atom.character = char_ctrl::cc_vtab;	//  '\v' 0x0b:VT
			break;

		case char_alnum::ch_f:
			atom.character = char_ctrl::cc_ff;	//  '\f' 0x0c:FF
			break;

		case char_alnum::ch_r:
			atom.character = char_ctrl::cc_cr;	//  '\r' 0x0d:CR
			break;

#if 0
		case char_alnum::ch_e:	//  \e
			atom.character = char_ctrl::cc_esc;	//  '\x1b' 0x1b:ESC
			break;
#endif

		case char_alnum::ch_c:	//  \cX
			if (curpos != last)
			{
//				atom.character = static_cast<uchar21_t>(this->traits_inst.codepoint_inc(curpos, last) & 0x1f);	//  *curpos++
				atom.character = static_cast<uchar21_t>(*curpos | 0x20);

				if (atom.character >= char_alnum::ch_a && atom.character <= char_alnum::ch_z)
					atom.character = static_cast<uchar21_t>(*curpos++ & 0x1f);
				else
				{
					this->throw_error(regex_constants::error_escape);	//  strict.
//					atom.character = char_alnum::ch_c;	//  loose.
				}
			}
			break;

		case char_alnum::ch_0:
			atom.character = char_ctrl::cc_nul;	//  '\0' 0x00:NUL
			break;

		case char_alnum::ch_u:	//  \uhhhh, \u{h~hhhhhh}
			if (curpos != last && static_cast<uchar21_t>(*curpos) == meta_char::mc_cbraop)
				atom.character = translate_numbers(++curpos, last, 16, 1, 6, constants::unicode_max_codepoint, true);
			else
				atom.character = translate_numbers(curpos, last, 16, 4, 4, 0xffff, false);
			break;

		case char_alnum::ch_x:	//  \xhh
			atom.character = translate_numbers(curpos, last, 16, 2, 2, 0xff, false);
			break;

		default:
			;
		}

		if (atom.character == constants::invalid_u21value)	//  static_cast<uchar21_t>(~0))
			this->throw_error(regex_constants::error_escape);

		return true;
	}

	//  Escape characters which do not appear in [] pairs.

	template <typename InputIterator>
	bool translate_atom_escape(state_type &atom, InputIterator &curpos, const InputIterator &last)
	{
		if (curpos == last)
			this->throw_error(regex_constants::error_escape);

		atom.character = static_cast<uchar21_t>(*curpos);

		switch (atom.character)
		{

		case char_alnum::ch_B:	//  'B':
			atom.is_not = true;

		case char_alnum::ch_b:	//  'b':
			atom.type   = st_boundary;	//  \b, \B.
			atom.quantifier.reset(0);
//			atom.number = 0;
			break;

//		case char_alnum::ch_A:	//  'A':
//			atom.type   = st_bol;	//  '\A'
//		case char_alnum::ch_Z:	//  'Z':
//			atom.type   = st_eol;	//  '\Z'
//		case char_alnum::ch_z:	//  'z':
//			atom.type   = st_eol;	//  '\z'
///		case char_alnum::ch_G:	//  'G':
//			atom.type   = st_previous_match;	//  '\G'

		//  backreferences.

		default:
			assert(atom.type == st_character);

			if (atom.character >= char_alnum::ch_1 && atom.character <= char_alnum::ch_9)
			{
				atom.number = static_cast<unsigned int>(translate_numbers(curpos, last, 10, 0, 0, 0, false));

				if (atom.number == static_cast<unsigned int>(constants::invalid_u21value))	//  ~0))
					this->throw_error(regex_constants::error_escape);
				else if (atom.number >= this->number_of_brackets)
					this->throw_error(regex_constants::error_backref);

				atom.next2 = 1;
				atom.type = st_backreference;
				atom.quantifier.set_infinity();
				return true;
			}
			return translate_escseq(atom, curpos, last);
		}

		++curpos;
		return true;
	}

	template <typename InputIterator>
	bool get_quantifier(re_quantifier &quantifier, InputIterator &curpos, const InputIterator &last)
	{

		switch (*curpos)
		{
		case meta_char::mc_astrsk:	//  '*':
			--quantifier.atleast;

		case meta_char::mc_plus:	//  '+':
			quantifier.set_infinity();
			break;

		case meta_char::mc_query:	//  '?':
			--quantifier.atleast;
			break;

		case meta_char::mc_cbraop:	//  '{':
			if (!get_brace_with_quantifier(quantifier, curpos, last))
				return false;
			break;

		default:
			return true;
		}

		if (++curpos != last && static_cast<uchar21_t>(*curpos) == meta_char::mc_query)	//  '?'
		{
			quantifier.is_greedy = false;
			++curpos;
		}
		return true;
	}

	template <typename InputIterator>
	bool get_brace_with_quantifier(re_quantifier &quantifier, InputIterator &curpos, const InputIterator &last)
	{

		++curpos;

		quantifier.atleast = static_cast<unsigned int>(translate_numbers(curpos, last, 10, 1, 0, 0, false));
		if (quantifier.atleast == static_cast<unsigned int>(constants::invalid_u21value))
			this->throw_error(regex_constants::error_brace);

		quantifier.atmost = quantifier.atleast;

		if (curpos == last)
			this->throw_error(regex_constants::error_brace);

		if (static_cast<uchar21_t>(*curpos) == meta_char::mc_comma)	//  ','
		{
			++curpos;

			quantifier.atmost = static_cast<unsigned int>(translate_numbers(curpos, last, 10, 1, 0, 0, false));

			if (quantifier.atmost == static_cast<unsigned int>(constants::invalid_u21value))
				quantifier.set_infinity();

			if (!quantifier.is_valid())
				this->throw_error(regex_constants::error_badbrace);
		}

		if (curpos == last || static_cast<uchar21_t>(*curpos) != meta_char::mc_cbracl)	//  '}'
			this->throw_error(regex_constants::error_brace);

		//  *curpos == '}'

		return true;
	}

	template <typename InputIterator>
	uchar21_t translate_numbers(InputIterator &begin, const InputIterator &end, const int radix, const std::size_t minsize, const std::size_t maxsize, const uchar21_t maxcodepoint, const bool needs_closecurlybracket)
	{
		uchar21_t univalue = 0;
		int num;

		for (std::size_t count = 0; !maxsize || count < maxsize; ++begin, ++count)
		{
			if (begin == end || (num = this->traits_inst.value(*begin, radix)) == -1)
			{
				if (count >= minsize)
					break;	//  OK.

				return constants::invalid_u21value;
			}
			univalue *= radix;
			univalue += num;
		}

		if (needs_closecurlybracket)
		{
			if (begin == end || static_cast<uchar21_t>(*begin) != meta_char::mc_cbracl)
				return constants::invalid_u21value;

			++begin;
		}
		if (!maxcodepoint || univalue <= maxcodepoint)
			return univalue;

		return constants::invalid_u21value;
	}

	//  check /(\1)/.
	bool check_backreference_wrapped_in_self() const
	{
		for (typename state_array::size_type backrefpos = 0; backrefpos < this->NFA_states.size(); ++backrefpos)
		{
			const state_type &brs = this->NFA_states[backrefpos];

			if (brs.type == st_backreference)
			{
				const unsigned int &backrefno = brs.number;
				typename state_array::size_type roundbracket_closepos = backrefpos;

				for (;;)
				{
					if (roundbracket_closepos)
					{
						const state_type &rbcs = this->NFA_states[--roundbracket_closepos];

						if (rbcs.type == st_roundbracket_close && rbcs.number == backrefno)
							break;
					}
					else
						return false;
				}
			}
		}
		return true;
	}

#if !defined(SRELLDBG_NO_1STCHRCLS)

	void create_firstchar_class()
	{
#if !defined(SRELLDBG_NO_BITSET)
		code_range_array fcc;
#else
		code_range_array &fcc = this->firstchar_class;
#endif
		simple_array<bool> checked;

		checked.resize(this->NFA_states.size(), false);

		gather_nextchars(fcc, static_cast<typename state_array::size_type>(this->NFA_states[0].next1), checked, 0);

		if (!fcc.size())
		{
			fcc.push_back(code_range_type(0, constants::unicode_max_codepoint));
			//  Expression would consist of assertions only, such as /^$/.
			//  We cannot but accept every codepoint.
		}
		else if (this->flags_ & regex_constants::icase)
#if !defined(SRELLDBG_NO_BITSET)
			make_caseunfoldedcharset(fcc);
#else
			make_charcls_nocase(fcc);
#endif

		fcc.optimise();

#if !defined(SRELLDBG_NO_BITSET)
		set_bitset_table(fcc);
#endif
	}

#if !defined(SRELLDBG_NO_BITSET)
	void set_bitset_table(const code_range_array &fcc)
	{
		this->firstchar_class_bs.reset();

		for (typename code_range_array::size_type i = 0; i < fcc.size(); ++i)
		{
			const code_range_type &range = fcc[i];

			for (uchar21_t ucp = range.first;; ++ucp)
			{
				this->firstchar_class_bs.set(ucp);

				if (ucp == range.second)
					break;
			}
		}
	}
#endif	//  !defined(SRELLDBG_NO_BITSET)

#endif	//  !defined(SRELLDBG_NO_1STCHRCLS)

	void gather_nextchars(code_range_array &nextcharclass, typename state_array::size_type pos, simple_array<bool> &checked, const unsigned int bracket_number) const
	{
		for (;;)
		{
			const state_type &state = this->NFA_states[pos];

			if (checked[pos])
				break;

			checked[pos] = true;

			if (state.next2)
				gather_nextchars(nextcharclass, pos + state.next2, checked, 0);

			switch (state.type)
			{
			case st_character:
				nextcharclass.push_back(code_range_type(state.character));
				return;

			case st_character_class:
				{
					code_range_array array = this->character_class[state.number];

					if (state.is_not)
						array.negation();

					for (typename code_range_array::size_type i = 0; i < array.size(); ++i)
						nextcharclass.push_back(array[i]);
				}
				return;

			case st_backreference:
				{
					const typename state_array::size_type nextpos = find_next_of_bracketopen(state.number);

					if (!check_if_wrapped_in_negative_lookaround(nextpos))
						gather_nextchars(nextcharclass, nextpos, checked, state.number);
				}
				break;

			case st_lookaround_open:

#if !defined(SRELL_NO_REVERSE)
//				if (!state.is_not && !state.reverse)
				if (!state.is_not && state.quantifier.atleast == 0)
#else
				if (!state.is_not)
#endif
					gather_nextchars(nextcharclass, pos + 2, checked, 0);

				break;

			case st_roundbracket_close:
				if (/* !bracket_number || */ state.number != bracket_number)
					break;

			case st_lookaround_pop:
			case st_success:	//  == st_lookaround_close.
				return;

			default:;
			}

			if (state.next1)
				pos += state.next1;
			else
				break;
		}
	}

	typename state_array::size_type find_next_of_bracketopen(const unsigned int bracketno) const
	{
		for (typename state_array::size_type no = 0; no < this->NFA_states.size(); ++no)
		{
			const state_type &state = this->NFA_states[no];

			if (state.type == st_roundbracket_open && state.number == bracketno)
				return no + state.next1;
		}
		assert(0);
		return 0;
	}

	bool check_if_wrapped_in_negative_lookaround(const typename state_array::size_type end) const
	{
		bool wrapped_in_negative_assertion = false;
		unsigned int depth = 0;

		for (typename state_array::size_type cur = 0; cur != end; ++cur)
		{
			const state_type &state = this->NFA_states[cur];

			if (state.type == st_lookaround_open)
			{
				++depth;

				if (state.is_not)
					wrapped_in_negative_assertion = true;
			}
			else if (state.type == st_lookaround_close)
			{
				if (--depth == 0)
					wrapped_in_negative_assertion = false;
			}
		}
		return wrapped_in_negative_assertion;
	}

//#endif	//  !SRELLDBG_NO_1STCHRCLS

	void relativejump_to_absolutejump()
	{
		for (typename state_array::size_type pos = 0; pos < this->NFA_states.size(); ++pos)
		{
			state_type &state = this->NFA_states[pos];

#if !defined(SRELLDBG_NO_ASTERISK_OPT)
			if (state.next1 || state.type == st_character || state.type == st_character_class)
#else
			if (state.next1)
#endif
				state.next_state1 = &this->NFA_states[pos + state.next1];
			else
				state.next_state1 = NULL;

			if (state.next2)
				state.next_state2 = &this->NFA_states[pos + state.next2];
			else
				state.next_state2 = NULL;
		}
	}

	void optimise()
	{
#if !defined(SRELLDBG_NO_ASTERISK_OPT)
		asterisk_optimisation();

  #if !defined(SRELLDBG_NO_NEXTPOS_OPT)
  		if (!check_if_backref_presents())
			nextpos_optimisation();
  #endif
#endif

#if !defined(SRELLDBG_NOT_SKIP_EPSILON)
		skip_epsilon();
#endif

#if !defined(SRELLDBG_NO_1STCHRCLS)
		create_firstchar_class();
#endif
	}

#if !defined(SRELLDBG_NOT_SKIP_EPSILON)

	void skip_epsilon()
	{
		for (typename state_array::size_type pos = 0; pos < this->NFA_states.size(); ++pos)
		{
			state_type &state = this->NFA_states[pos];

			if (state.next1)
				state.next1 = static_cast<ptrdiff_t>(skip_nonbranch_epsilon(pos + state.next1) - pos);

			if (state.next2)
				state.next2 = static_cast<ptrdiff_t>(skip_nonbranch_epsilon(pos + state.next2) - pos);
		}
	}

	typename state_array::size_type skip_nonbranch_epsilon(typename state_array::size_type pos) const
	{
		for (;;)
		{
			const state_type &state = this->NFA_states[pos];

			if (state.type == st_epsilon && state.next2 == 0)
			{
				pos += state.next1;
				continue;
			}
			break;
		}
		return pos;
	}

#endif

#if !defined(SRELLDBG_NO_ASTERISK_OPT)

	void asterisk_optimisation()
	{
		for (typename state_array::size_type cur = 0; cur != this->NFA_states.size(); ++cur)
		{
			state_type &curstate = this->NFA_states[cur];

			switch (curstate.type)
			{
			case st_character:
			case st_character_class:
				if (curstate.quantifier.atleast == 0 && curstate.quantifier.is_infinity())
				{
					if (is_exclusive_sequence(cur))
					{
						state_type &prevstate = this->NFA_states[cur - 1];

						assert(prevstate.type == st_epsilon);

						prevstate.next1 = 1;
						prevstate.next2 = 0;
//						prevstate.quantifier.is_greedy = true;
//						curstate.quantifier.is_greedy = true;
						curstate.next1 = 0;
						curstate.next2 = 1;
					}
				}
				break;

				//  check_counter, epsilon x 4, increment_counter, decrement_counter, char_or_class
			case st_check_counter:
				if (!curstate.quantifier.is_same() && is_exclusive_sequence(cur + 3) && insert_at(cur + 1, 4))
					split_counter(cur);

				break;

			default:;
			}
		}
	}

	bool is_exclusive_sequence(const typename state_array::size_type cur) const
	{
		const state_type &curstate = this->NFA_states[cur];
		code_range_array curchar_class;
		code_range_array nextchar_class;
		simple_array<bool> checked;

		checked.resize(this->NFA_states.size(), false);

		if (curstate.type == st_character)
		{
			curchar_class.push_back(code_range_type(curstate.character));
		}
		else if (curstate.type == st_character_class)
		{
			curchar_class = this->character_class[curstate.number];
			if (curstate.is_not)
				curchar_class.negation();
		}
		else
		{
			return false;
		}

		gather_nextchars(nextchar_class, cur + 1, checked, 0);

		if (nextchar_class.size() && !curchar_class.is_overlap(nextchar_class))
		{
			return true;
		}
		return false;
	}

	bool insert_at(const typename state_array::size_type pos, const ptrdiff_t len)
	{
		state_type newstate;

		for (typename state_array::size_type cur = 0; cur < pos; ++cur)
		{
			state_type &state = this->NFA_states[cur];

			if (state.next1 && (cur + state.next1) >= pos)
				state.next1 += len;

			if (state.next2 && (cur + state.next2) >= pos)
				state.next2 += len;
		}

		for (typename state_array::size_type cur = pos; cur < this->NFA_states.size(); ++cur)
		{
			state_type &state = this->NFA_states[cur];

			if ((cur + state.next1) < pos)
				state.next1 -= len;

			if ((cur + state.next2) < pos)
				state.next2 -= len;
		}

		newstate.reset();
		newstate.type = st_epsilon;
		for (ptrdiff_t count = 0; count < len; ++count)
			this->NFA_states.insert(pos, newstate);

		return true;
	}

	void split_counter(typename state_array::size_type &cur)
	{
		state_type &cur_chkcnt = this->NFA_states[cur];	//  check_counter
		state_type &new_inccnt = this->NFA_states[cur + 1];	//  copy of increment_counter (cur_inccnt).
		state_type &new_deccnt = this->NFA_states[cur + 2];	//  copy of decrement_counter (cur_deccnt).
		state_type &new_chorcl = this->NFA_states[cur + 3];	//  copy of char or class (cur_chorcl).
		state_type &new_chkcnt = this->NFA_states[cur + 4];	//  copy of check_counter (cur).
		state_type &cur_inccnt = this->NFA_states[cur + 5];	//  increment_counter
		state_type &cur_deccnt = this->NFA_states[cur + 6];	//  decrement_counter
		state_type &cur_chorcl = this->NFA_states[cur + 7];	//  character or class.

		assert(new_inccnt.type == st_epsilon);
		assert(new_deccnt.type == st_epsilon);
		assert(new_chorcl.type == st_epsilon);
		assert(new_chkcnt.type == st_epsilon);

		//  Cuts relationship.
		cur_chkcnt.next1 -= 4;
		cur_chkcnt.next2 -= 4;
		cur_chorcl.next1 += 4;
//		cur_chorcl.next2 = 0;

		new_chkcnt = cur_chkcnt;

		if (new_chkcnt.quantifier.atleast <= 4)
		{
			if (new_chkcnt.quantifier.atleast)
			{
				cur_chkcnt = cur_chorcl;
				cur_chkcnt.next1 = 1;
//				cur_chkcnt.next2 = 0;
				cur_chkcnt.quantifier.reset();

				if (new_chkcnt.quantifier.atleast > 1)
				{
					new_inccnt = cur_chkcnt;
					if (new_chkcnt.quantifier.atleast > 2)
					{
						new_deccnt = cur_chkcnt;
						if (new_chkcnt.quantifier.atleast > 3)
							new_chorcl = cur_chkcnt;
					}
				}
			}
			else
			{
				cur_chkcnt.reset();
				cur_chkcnt.type = st_epsilon;
			}

			if (!new_chkcnt.quantifier.is_infinity())
				new_chkcnt.quantifier.atmost -= new_chkcnt.quantifier.atleast;

			new_chkcnt.quantifier.atleast = 0;
		}
		else
		{
			cur_chkcnt.next1 = 1;
			cur_chkcnt.next2 = 4;
			cur_chkcnt.quantifier.is_greedy = true;
			cur_chkcnt.quantifier.atmost = cur_chkcnt.quantifier.atleast;

			new_inccnt = cur_inccnt;	//  increment_counter.
			new_deccnt = cur_deccnt;	//  decrement_counter.
			new_chorcl = cur_chorcl;	//  character or character_class.
		}
		new_chkcnt.dont_push = true;	//  Disables backtracking.
		cur_chorcl.next2 = 1;
		cur += 4;
	}

#if !defined(SRELLDBG_NO_NEXTPOS_OPT)

	bool check_if_backref_presents(typename state_array::size_type begin = 0, const unsigned int number = 0) const
	{
		for (; begin < this->NFA_states.size(); ++begin)
		{
			const state_type &state = this->NFA_states[begin];

			if (state.type == st_backreference && (number == 0 || state.number == number))
				return true;
		}
		return false;
	}

	void nextpos_optimisation()
	{
		typename state_array::size_type cur = 0;
		state_type *prevstate = NULL;

		for (;;)
		{
			state_type &curstate = this->NFA_states[cur];

			switch (curstate.type)
			{
			case st_character:
			case st_character_class:
				if (prevstate && !is_sequence(prevstate, curstate))
					break;

				//  !prevstate || is_sequence()
				if (curstate.next1 == 0 && insert_at(cur + 1, 1))
				{
					state_type &newstate = this->NFA_states[cur + 1];

					newstate.type = st_move_nextpos;
					newstate.next1 = curstate.next2 - 1;
					curstate.next2 = 1;
					return;
				}
				if (prevstate)
					break;

				prevstate = &curstate;
				cur += curstate.next1;
				continue;

			case st_save_and_reset_counter:
			case st_success:
				break;

			case st_epsilon:
				if (curstate.next2)
					break;

			default:
				cur += curstate.next1;
				continue;
			}
			break;
		}
	}

	bool is_sequence(const state_type *prevstate, const state_type &curstate) const
	{
		if (prevstate->type == curstate.type)
		{
			if (prevstate->type == st_character && prevstate->character == curstate.character)
				return true;

			if (prevstate->type == st_character_class && prevstate->number == curstate.number && prevstate->is_not == curstate.is_not)
				return true;
		}
		return false;
	}

#endif	//  !defined(SRELLDBG_NO_NEXTPOS_OPT)
#endif	//  !defined(SRELLDBG_NO_ASTERISK_OPT)

public:	//  for debug.

	void print_NFA_states(const bool) const;
};
//  re_compiler

	}	//  namespace regex_internal

//  ... "rei_compiler.hpp"]
//  ["rei_search_state.hpp" ...

template <typename BidirectionalIterator>
class sub_match /* : std::pair<BidirectionalIterator, BidirectionalIterator> */;

	namespace regex_internal
	{

//template <typename charT>
struct re_state;

template </* typename charT, */typename BidirectionalIterator>
struct re_search_state_core
{
	const re_state/* <charT> */ *in_NFA_states;
	BidirectionalIterator in_string;
};

template <typename BidirectionalIterator>
struct re_submatch_core
{
	BidirectionalIterator open_at;
	BidirectionalIterator close_at;
};

template <typename BidirectionalIterator>
struct re_submatch_type
{
	re_submatch_core<BidirectionalIterator> core;
	unsigned int counter;
};

template </*typename charT, */typename BidirectionalIterator>
struct re_search_state_types
{
public:

	typedef re_submatch_core<BidirectionalIterator> submatch_core;
	typedef re_submatch_type<BidirectionalIterator> submatch_type;
	typedef BidirectionalIterator position_type;
	typedef unsigned int counter_type;

	typedef std::vector<submatch_type> submatch_array;

	typedef re_search_state_core</*charT, */BidirectionalIterator> search_core_state;

public:

	typedef std::vector<search_core_state> backtracking_array;
	typedef std::vector<submatch_core> capture_array;
	typedef std::vector<position_type> repeat_array;
	typedef simple_array<counter_type> counter_array;
};

template </*typename charT1, */typename charT2>
struct re_search_state_types</*charT1, */const charT2 *>
{
public:

	typedef re_submatch_core<const charT2 *> submatch_core;
	typedef re_submatch_type<const charT2 *> submatch_type;
	typedef const charT2 *position_type;
	typedef unsigned int counter_type;

	typedef simple_array<submatch_type> submatch_array;

	typedef re_search_state_core</*charT1, */const charT2 *> search_core_state;

public:

	typedef simple_array<search_core_state> backtracking_array;
	typedef simple_array<submatch_core> capture_array;
//	typedef typename capture_array::size_type history_size;
	typedef simple_array<position_type> repeat_array;
	typedef simple_array<counter_type> counter_array;
};
//  re_search_state_types

template </*typename charT, */typename BidirectionalIterator>
class re_search_state : public re_search_state_types</*charT, */BidirectionalIterator>
{
private:

	typedef re_search_state_types</*charT, */BidirectionalIterator> base_type;

public:

	typedef typename base_type::submatch_core submatch_core;
	typedef typename base_type::submatch_type submatch_type;
	typedef typename base_type::position_type position_type;
	typedef typename base_type::counter_type counter_type;

	typedef typename base_type::submatch_array submatch_array;

	typedef typename base_type::search_core_state search_core_state;

public:

	typedef typename base_type::backtracking_array backtracking_array;
	typedef typename base_type::capture_array capture_array;
	typedef typename base_type::repeat_array repeat_array;
	typedef typename base_type::counter_array counter_array;

	typedef typename backtracking_array::size_type btstack_size_type;

public:

	struct bottom_state
	{
		btstack_size_type btstack_size;
		typename capture_array::size_type capturestack_size;
		typename counter_array::size_type counterstack_size;
		typename repeat_array::size_type repeatstack_size;

		bottom_state(
			const btstack_size_type &bt,
			const typename capture_array::size_type &h,
			const typename counter_array::size_type &c,
			const typename repeat_array::size_type &r)
			: btstack_size(bt)
			, capturestack_size(h)
			, counterstack_size(c)
			, repeatstack_size(r)
		{
		}
	};

#if defined(SRELL_NO_RECURSIVE)
	typedef simple_array<bottom_state> stacks_stack_type;
#endif

public:

	search_core_state nth;

#if !defined(SRELL_NO_LIMIT_COUNTER)
	std::size_t failure_counter;
#endif

	backtracking_array bt_stack;

	capture_array capture_stack;
	counter_array counter_stack;
	repeat_array repeat_stack;

	submatch_array bracket;
	counter_array counter;
	repeat_array repeat;

	btstack_size_type btstack_size;

	BidirectionalIterator nextpos;

//	BidirectionalIterator backrefpos;

	/*const */BidirectionalIterator base;
	/*const */BidirectionalIterator last;

#if defined(SRELL_NO_RECURSIVE)
	stacks_stack_type stacks_stack;
#endif

public:

	void init
	(
		const BidirectionalIterator &b,
		const BidirectionalIterator &l,
		const regex_constants::match_flag_type &f,
		const re_state/*<charT>*/ *const &entry,
		unsigned int num_of_submatches,
		const unsigned int &num_of_counters,
		const unsigned int &num_of_repeats
	)
	{
		base = nextpos = b;
		last = l;
		flags_ = f & ~regex_constants::match_last_tried_;
		entry_state_ = entry;
		btstack_size = 0;

		bracket.resize(num_of_submatches);
		counter.resize(num_of_counters);
		repeat.resize(num_of_repeats);

		clear_stacks();

		while (num_of_submatches)
		{
			submatch_type &br = bracket[--num_of_submatches];

			br.core.open_at = br.core.close_at = this->last;
			br.counter = 0;
			//  15.10.2.9; AtomEscape:
			//  If the regular expression has n or more capturing parentheses
			//  but the nth one is undefined because it hasn't captured anything,
			//  then the backreference always succeeds.

			//  C.f., table 27 and 28 on TR1, table 142 and 143 on C++11.
		}
	}

#if defined(SRELL_NO_LIMIT_COUNTER)
	void reset(const BidirectionalIterator &start)
#else
	void reset(const BidirectionalIterator &start, const std::size_t &limit)
#endif
	{
		nth.in_NFA_states = this->entry_state_;
		nth.in_string = start;

		bracket[0].core.open_at = start;

#if !defined(SRELL_NO_LIMIT_COUNTER)
		failure_counter = limit;
#endif
	}

	bool is_base() const
	{
		return nth.in_string == this->base;
	}

	bool is_last() const
	{
		return nth.in_string == this->last;
	}

	bool is_null() const
	{
		return nth.in_string == bracket[0].core.open_at;
	}

//	regex_constants::match_flag_type flags() const
//	{
//		return this->flags_;
//	}

	bool match_not_bol_flag() const
	{
		if (this->flags_ & regex_constants::match_not_bol)
			return true;
		return false;
	}

	bool match_not_eol_flag() const
	{
		if (this->flags_ & regex_constants::match_not_eol)
			return true;
		return false;
	}

	bool match_not_bow_flag() const
	{
		if (this->flags_ & regex_constants::match_not_bow)
			return true;
		return false;
	}

	bool match_not_eow_flag() const
	{
		if (this->flags_ & regex_constants::match_not_eow)
			return true;
		return false;
	}

	bool match_prev_avail_flag() const
	{
		if (this->flags_ & regex_constants::match_prev_avail)
			return true;
		return false;
	}

	bool match_not_null_flag() const
	{
		if (this->flags_ & regex_constants::match_not_null)
			return true;
		return false;
	}

	bool match_match_flag() const
	{
		if (this->flags_ & regex_constants::match_match_)
			return true;
		return false;
	}

	bool is_last_tried()
	{
		if (this->flags_ & regex_constants::match_last_tried_)
			return true;

		this->flags_ |= regex_constants::match_last_tried_;
		return false;
	}

	void set_submatch(sub_match<BidirectionalIterator> &sm, const unsigned int no) const
	{
		const submatch_type &br = bracket[no];

#if defined(SRELLTEST_PUSHPOP_OPT)
		sm.matched = br.counter != 0;
		if (sm.matched || !no)
		{
			sm.first  = br.core.open_at;
			sm.second = br.core.close_at;
		}
		else
			sm.first = sm.second = this->last;
			//  C.f., table 27 and 28 on TR1, table 142 and 143 on C++11.
#else
		sm.first   = br.core.open_at;
		sm.second  = br.core.close_at;
		sm.matched = br.counter != 0;
#endif
	}

	void clear_stacks()
	{
		btstack_size = 0;
		bt_stack.clear();
		capture_stack.clear();
		repeat_stack.clear();
		counter_stack.clear();

#if defined(SRELL_NO_RECURSIVE)
		stacks_stack.clear();
#endif
	}

	btstack_size_type size() const	//  for debug.
	{
		return bt_stack.size();
	}

	bool is_empty() const	//  for debug.
	{
		if (btstack_size == 0
			&& bt_stack.size() == 0
			&& capture_stack.size() == 0
			&& repeat_stack.size() == 0
			&& counter_stack.size() == 0)
			return true;

		return false;
	}

	re_search_state &operator=(const re_search_state &)
	{
		throw std::runtime_error("do not copy an object of class `re_search_state'.");
		return *this;
	}

private:

	/* const */regex_constants::match_flag_type flags_;
	const re_state/* <charT> */ * /* const */entry_state_;
};
//  re_search_state

	}	//  namespace regex_internal

//  ... "rei_search_state.hpp"]
//  ["regex_sub_match.hpp" ...

//  28.9, class template sub_match:
template <class BidirectionalIterator>
class sub_match : public std::pair<BidirectionalIterator, BidirectionalIterator>
{
public:

	typedef typename std::iterator_traits<BidirectionalIterator>::value_type value_type;
	typedef typename std::iterator_traits<BidirectionalIterator>::difference_type difference_type;
	typedef BidirectionalIterator iterator;
	typedef std::basic_string<value_type> string_type;

	bool matched;

//	constexpr sub_match();	//  C++11.

	sub_match() : matched(false)
	{
	}

	difference_type length() const
	{
		return matched ? std::distance(this->first, this->second) : 0;
	}

	operator string_type() const
	{
		return matched ? string_type(this->first, this->second) : string_type();
	}

	string_type str() const
	{
		return matched ? string_type(this->first, this->second) : string_type();
	}

	int compare(const sub_match &s) const
	{
		return str().compare(s.str());
	}

	int compare(const string_type &s) const
	{
		return str().compare(s);
	}

	int compare(const value_type *const s) const
	{
		return str().compare(s);
	}
};

//  28.9.2, sub_match non-member operators:
//  [7.9.2] sub_match non-member operators

//  compare sub_match & with sub_match &.
template <class BiIter>
bool operator==(const sub_match<BiIter> &lhs, const sub_match<BiIter> &rhs)
{
	return lhs.compare(rhs) == 0;	//  1
}

template <class BiIter>
bool operator!=(const sub_match<BiIter> &lhs, const sub_match<BiIter> &rhs)
{
	return lhs.compare(rhs) != 0;	//  2
}

template <class BiIter>
bool operator<(const sub_match<BiIter> &lhs, const sub_match<BiIter> &rhs)
{
	return lhs.compare(rhs) < 0;	//  3
}

template <class BiIter>
bool operator<=(const sub_match<BiIter> &lhs, const sub_match<BiIter> &rhs)
{
	return lhs.compare(rhs) <= 0;	//  4
}

template <class BiIter>
bool operator>=(const sub_match<BiIter> &lhs, const sub_match<BiIter> &rhs)
{
	return lhs.compare(rhs) >= 0;	//  5
}

template <class BiIter>
bool operator>(const sub_match<BiIter> &lhs, const sub_match<BiIter> &rhs)
{
	return lhs.compare(rhs) > 0;	//  6
}

//  compare basic_string & with sub_match &.
template <class BiIter, class ST, class SA>
bool operator==(
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &lhs,
	const sub_match<BiIter> &rhs
)
{
	return rhs.compare(lhs.c_str()) == 0;	//  7
}

template <class BiIter, class ST, class SA>
bool operator!=(
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &lhs,
	const sub_match<BiIter> &rhs
)
{
	return !(lhs == rhs);	//  8
}

template <class BiIter, class ST, class SA>
bool operator<(
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &lhs,
	const sub_match<BiIter> &rhs
)
{
	return rhs.compare(lhs.c_str()) > 0;	//  9
}

template <class BiIter, class ST, class SA>
bool operator>(
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &lhs,
	const sub_match<BiIter> &rhs
)
{
	return rhs < lhs;	//  10
}

template <class BiIter, class ST, class SA>
bool operator>=(
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &lhs,
	const sub_match<BiIter> &rhs
)
{
	return !(lhs < rhs);	//  11
}

template <class BiIter, class ST, class SA>
bool operator<=(
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &lhs,
	const sub_match<BiIter> &rhs
)
{
	return !(rhs < lhs);	//  12
}

//  compare sub_match & with basic_string &.
template <class BiIter, class ST, class SA>
bool operator==(
	const sub_match<BiIter> &lhs,
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &rhs
)
{
	return lhs.compare(rhs.c_str()) == 0;	//  13
}

template <class BiIter, class ST, class SA>
bool operator!=(
	const sub_match<BiIter> &lhs,
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &rhs
)
{
	return !(lhs == rhs);	//  14
}

template <class BiIter, class ST, class SA>
bool operator<(
	const sub_match<BiIter> &lhs,
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &rhs
)
{
	return lhs.compare(rhs.c_str()) < 0;	//  15
}

template <class BiIter, class ST, class SA>
bool operator>(
	const sub_match<BiIter> &lhs,
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &rhs
)
{
	return rhs < lhs;	//  16
}

template <class BiIter, class ST, class SA>
bool operator>=(
	const sub_match<BiIter> &lhs,
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &rhs
)
{
	return !(lhs < rhs);	//  17
}

template <class BiIter, class ST, class SA>
bool operator<=(
	const sub_match<BiIter> &lhs,
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &rhs
)
{
	return !(rhs < lhs);	//  18
}

//  compare iterator_traits::value_type * with sub_match &.
template <class BiIter>
bool operator==(
	typename std::iterator_traits<BiIter>::value_type const *lhs,
	const sub_match<BiIter> &rhs
)
{
	return rhs.compare(lhs) == 0;	//  19
}

template <class BiIter>
bool operator!=(
	typename std::iterator_traits<BiIter>::value_type const *lhs,
	const sub_match<BiIter> &rhs
)
{
	return !(lhs == rhs);	//  20
}

template <class BiIter>
bool operator<(
	typename std::iterator_traits<BiIter>::value_type const *lhs,
	const sub_match<BiIter> &rhs
)
{
	return rhs.compare(lhs) > 0;	//  21
}

template <class BiIter>
bool operator>(
	typename std::iterator_traits<BiIter>::value_type const *lhs,
	const sub_match<BiIter> &rhs
)
{
	return rhs < lhs;	//  22
}

template <class BiIter>
bool operator>=(
	typename std::iterator_traits<BiIter>::value_type const *lhs,
	const sub_match<BiIter> &rhs
)
{
	return !(lhs < rhs);	//  23
}

template <class BiIter>
bool operator<=(
	typename std::iterator_traits<BiIter>::value_type const *lhs,
	const sub_match<BiIter> &rhs
)
{
	return !(rhs < lhs);	//  24
}

//  compare sub_match & with iterator_traits::value_type *.
template <class BiIter>
bool operator==(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const *rhs
)
{
	return lhs.compare(rhs) == 0;	//  25
}

template <class BiIter>
bool operator!=(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const *rhs
)
{
	return !(lhs == rhs);	//  26
}

template <class BiIter>
bool operator<(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const *rhs
)
{
	return lhs.compare(rhs) < 0;	//  27
}

template <class BiIter>
bool operator>(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const *rhs
)
{
	return rhs < lhs;	//  28
}

template <class BiIter>
bool operator>=(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const *rhs
)
{
	return !(lhs < rhs);	//  29
}

template <class BiIter>
bool operator<=(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const *rhs
)
{
	return !(rhs < lhs);	//  30
}

//  compare iterator_traits::value_type & with sub_match &.
template <class BiIter>
bool operator==(
	typename std::iterator_traits<BiIter>::value_type const &lhs,
	const sub_match<BiIter> &rhs
)
{
	return rhs.compare(typename sub_match<BiIter>::string_type(1, lhs)) == 0;	//  31
}

template <class BiIter>
bool operator!=(
	typename std::iterator_traits<BiIter>::value_type const &lhs,
	const sub_match<BiIter> &rhs
)
{
	return !(lhs == rhs);	//  32
}

template <class BiIter>
bool operator<(
	typename std::iterator_traits<BiIter>::value_type const &lhs,
	const sub_match<BiIter> &rhs
)
{
	return rhs.compare(typename sub_match<BiIter>::string_type(1, lhs)) > 0;	//  33
}

template <class BiIter>
bool operator>(
	typename std::iterator_traits<BiIter>::value_type const &lhs,
	const sub_match<BiIter> &rhs
)
{
	return rhs < lhs;	//  34
}

template <class BiIter>
bool operator>=(
	typename std::iterator_traits<BiIter>::value_type const &lhs,
	const sub_match<BiIter> &rhs
)
{
	return !(lhs < rhs);	//  35
}

template <class BiIter>
bool operator<=(
	typename std::iterator_traits<BiIter>::value_type const &lhs,
	const sub_match<BiIter> &rhs
)
{
	return !(rhs < lhs);	//  36
}

//  compare sub_match & with iterator_traits::value_type &.
template <class BiIter>
bool operator==(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const &rhs
)
{
	return lhs.compare(typename sub_match<BiIter>::string_type(1, rhs)) == 0;	//  37
}

template <class BiIter>
bool operator!=(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const &rhs
)
{
	return !(lhs == rhs);	//  38
}

template <class BiIter>
bool operator<(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const &rhs
)
{
	return lhs.compare(typename sub_match<BiIter>::string_type(1, rhs)) < 0;	//  39
}

template <class BiIter>
bool operator>(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const &rhs
)
{
	return rhs < lhs;	//  40
}

template <class BiIter>
bool operator>=(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const &rhs
)
{
	return !(lhs < rhs);	//  41
}

template <class BiIter>
bool operator<=(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const &rhs
)
{
	return !(rhs < lhs);	//  42
}

template <class charT, class ST, class BiIter>
std::basic_ostream<charT, ST> &operator<<(std::basic_ostream<charT, ST> &os, const sub_match<BiIter> &m)
{
	return (os << m.str());
}

typedef sub_match<const char *> csub_match;
typedef sub_match<const wchar_t *> wcsub_match;
typedef sub_match<std::string::const_iterator> ssub_match;
typedef sub_match<std::wstring::const_iterator> wssub_match;

#if defined(SRELL_CPP11_CHAR1632_ENABLED)
	typedef sub_match<const char16_t *> u16csub_match;
	typedef sub_match<const char32_t *> u32csub_match;
	typedef sub_match<std::u16string::const_iterator> u16ssub_match;
	typedef sub_match<std::u32string::const_iterator> u32ssub_match;
#endif

#if defined(CHAR_BIT) && CHAR_BIT >= 8
	typedef csub_match u8csub_match;
	typedef ssub_match u8ssub_match;
#endif

#if defined(WCHAR_MAX)
	#if WCHAR_MAX >= 0x10ffff
		typedef wcsub_match u32wcsub_match;
		typedef wssub_match u32wssub_match;
	#elif WCHAR_MAX >= 0xffff
		typedef wcsub_match u16wcsub_match;
		typedef wssub_match u16wssub_match;
	#endif
#endif

//  ... "regex_sub_match.hpp"]
//  ["regex_match_results.hpp" ...

//  28.10, class template match_results:
template <class BidirectionalIterator, class Allocator = std::allocator<sub_match<BidirectionalIterator> > >
class match_results
{
public:

	typedef sub_match<BidirectionalIterator> value_type;
	typedef const value_type & const_reference;
	typedef const_reference reference;
//	typedef implementation defined const_iterator;
	typedef typename std::vector<value_type, Allocator>::const_iterator const_iterator;
	typedef const_iterator iterator;
	typedef typename std::iterator_traits<BidirectionalIterator>::difference_type difference_type;

#if defined(__cplusplus) && __cplusplus >= 201103L
	typedef typename std::allocator_traits<Allocator>::size_type size_type;	//  C++11.
#else
	typedef typename Allocator::size_type size_type;	//  TR1.
#endif

	typedef Allocator allocator_type;
	typedef typename std::iterator_traits<BidirectionalIterator>::value_type char_type;
	typedef std::basic_string<char_type> string_type;

public:

	//  28.10.1, construct/copy/destroy:
	//  [7.10.1] construct/copy/destroy
	explicit match_results(const Allocator &a = Allocator()) : ready_(false), sub_matches_(a)
	{
	}

	match_results(const match_results &m) : ready_(false)
	{
		operator=(m);
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	match_results(match_results &&m) noexcept	//  C++11.
	{
		operator=(std::move(m));
	}
#endif	//  SRELL_CPP11_MOVE_ENABLED

	match_results &operator=(const match_results &m)
	{
		if (this != &m)
		{
//			this->state_ = m.state_;
			this->ready_ = m.ready_;
			this->sub_matches_ = m.sub_matches_;
			this->prefix_ = m.prefix_;
			this->suffix_ = m.suffix_;
			this->base_ = m.base_;
		}
		return *this;
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	match_results &operator=(match_results &&m)	//  C++11.
	{
		if (this != &m)
		{
//			this->state_ = std::move(m.state_);
			this->ready_ = m.ready_;
			this->sub_matches_ = std::move(m.sub_matches_);
			this->prefix_ = std::move(m.prefix_);
			this->suffix_ = std::move(m.suffix_);
			this->base_ = m.base_;
		}
		return *this;
	}
#endif

//	~match_results();

	//  28.10.2, state:
	bool ready() const
	{
		return ready_;
	}

	//  28.10.3, size:
	//  [7.10.2] size
	size_type size() const
	{
		return sub_matches_.size();
	}

	size_type max_size() const
	{
		return sub_matches_.max_size();
//		return static_cast<size_type>(~0) / sizeof (value_type);
	}

	bool empty() const
	{
		return size() == 0;
	}

	//  28.10.4, element access:
	//  [7.10.3] element access
	difference_type length(const size_type sub = 0) const
	{
		return (*this)[sub].length();
	}

	difference_type position(const size_type sub = 0) const
	{
		const_reference ref = (*this)[sub];

		return std::distance(base_, ref.first);
	}

	string_type str(const size_type sub = 0) const
	{
		return string_type((*this)[sub]);
	}

	const_reference operator[](const size_type n) const
	{
		return sub_matches_[n];
	}

	const_reference prefix() const
	{
		return prefix_;
	}

	const_reference suffix() const
	{
		return suffix_;
	}

	const_iterator begin() const
	{
		return sub_matches_.begin();
	}

	const_iterator end() const
	{
		return sub_matches_.end();
	}

	const_iterator cbegin() const
	{
		return sub_matches_.begin();
	}

	const_iterator cend() const
	{
		return sub_matches_.end();
	}

	//  28.10.5, format:
	//  [7.10.4] format
	template <class OutputIter>
	OutputIter format(
		OutputIter out,
		const char_type *fmt_first,
		const char_type *const fmt_last,
		regex_constants::match_flag_type /* flags */ = regex_constants::format_default
	) const
	{
		if (this->ready() && !this->empty())
		{
			const value_type &m0 = (*this)[0];

			while (fmt_first != fmt_last)
			{
				if (*fmt_first != static_cast<char_type>(regex_internal::meta_char::mc_dollar))	//  '$'
				{
					*out++ = *fmt_first++;
				}
				else
				{
					++fmt_first;
					if (fmt_first == fmt_last)
					{
						*out++ = regex_internal::meta_char::mc_dollar;	//  '$';
					}
					else if (*fmt_first == static_cast<char_type>(regex_internal::char_other::co_amp))	//  '&', $&
					{
						out = std::copy(m0.first, m0.second, out);
						++fmt_first;
					}
					else if (*fmt_first == static_cast<char_type>(regex_internal::char_other::co_grav))	//  '`', $`, prefix.
					{
						out = std::copy(this->prefix().first, this->prefix().second, out);
						++fmt_first;
					}
					else if (*fmt_first == static_cast<char_type>(regex_internal::char_other::co_apos))	//  '\'', $', suffix.
					{
						out = std::copy(this->suffix().first, this->suffix().second, out);
						++fmt_first;
					}
					else
					{
						const char_type *const current_backup = fmt_first;
						size_type number = 0;

						while (fmt_first != fmt_last && *fmt_first >= static_cast<char_type>(regex_internal::char_alnum::ch_0) && *fmt_first <= static_cast<char_type>(regex_internal::char_alnum::ch_9))	//  '0'-'9'
						{
							number *= 10;
							number += *fmt_first - regex_internal::char_alnum::ch_0;	//  '0';
							if (number >= size())
							{
								number /= 10;
								break;
							}
							++fmt_first;
						}

						if (number)
						{
							const value_type &mn = (*this)[number];

							if (mn.matched)
								out = std::copy(mn.first, mn.second, out);
						}
						else
						{
							fmt_first = current_backup;
								//  '$', $$
							if (*fmt_first == static_cast<char_type>(regex_internal::meta_char::mc_dollar))
								++fmt_first;

							*out++ = regex_internal::meta_char::mc_dollar;	//  '$';
						}
					}
				}
			}
		}
		return out;
	}

	template <class OutputIter, class ST, class SA>
	OutputIter format(
		OutputIter out,
		const std::basic_string<char_type, ST, SA> &fmt,
		regex_constants::match_flag_type flags = regex_constants::format_default
	) const
	{
		return format(out, fmt.data(), fmt.data() + fmt.size(), flags);
	}

	template <class ST, class SA>
	std::basic_string<char_type, ST, SA> format(
		const string_type &fmt,
		regex_constants::match_flag_type flags = regex_constants::format_default
	) const
	{
		std::basic_string<char_type, ST, SA> result;

//		format(std::back_insert_iterator<string_type>(result), fmt, flags);
		format(std::back_inserter(result), fmt, flags);
		return result;
	}

	string_type format(const char_type *fmt, regex_constants::match_flag_type flags = regex_constants::format_default) const
	{
		string_type result;

		format(std::back_inserter(result), fmt, fmt + std::char_traits<char_type>::length(fmt), flags);
		return result;
	}

	//  28.10.6, allocator:
	//  [7.10.5] allocator
	allocator_type get_allocator() const
	{
		return allocator_type();
	}

	//  28.10.7, swap:
	//  [7.10.6] swap
	void swap(match_results &that)
	{
		const match_results tmp(that);
		that = *this;
		*this = tmp;
	}

public:	//  for internal.

	typedef match_results<BidirectionalIterator> match_results_type;
	typedef typename match_results_type::size_type match_results_size_type;
	typedef typename regex_internal::re_search_state</*charT, */BidirectionalIterator> search_state_type;

	search_state_type state_;

	void clear_()
	{
		ready_ = false;
		sub_matches_.clear();
//		prefix_.matched = false;
//		suffix_.matched = false;
	}

//	template <typename charT>
	bool set_match_results_()
	{
		unsigned int numofbrackets = static_cast<unsigned int>(state_.bracket.size());

		sub_matches_.resize(numofbrackets);

		while (numofbrackets)
		{
			value_type &sm = sub_matches_[--numofbrackets];

			state_.set_submatch(sm, numofbrackets);
		}

		sub_matches_[0].second  = state_.nth.in_string;
		sub_matches_[0].matched = true;

		prefix_.first   = base_ = state_.base;
		prefix_.second  = sub_matches_[0].first;
		prefix_.matched = prefix_.first != prefix_.second;
		//  The spec says prefix().first != prefix().second

		suffix_.first   = sub_matches_[0].second;
		suffix_.second  = state_.last;
		suffix_.matched = suffix_.first != suffix_.second;
		//  The spec says suffix().first != suffix().second

		ready_ = true;

		return true;
	}

	void set_base_(const BidirectionalIterator &base)
	{
		base_ = base;
	}

	bool mark_as_failed_()
	{
		//assert(state_.is_empty());
		ready_ = true;
		return false;
	}

public:	//  for debug.

	template <typename BasicRegexT>
	void print_sub_matches(const BasicRegexT &, const bool) const;
	void print_addresses(const value_type &, const char *const) const;

private:

	typedef std::vector<value_type, Allocator> sub_match_array;

	bool ready_;
	sub_match_array sub_matches_;
	value_type prefix_;
	value_type suffix_;
	BidirectionalIterator base_;
};

//  28.10.7, match_results swap:
//  [7.10.6] match_results swap
template <class BidirectionalIterator, class Allocator>
void swap(
	match_results<BidirectionalIterator, Allocator> &m1,
	match_results<BidirectionalIterator, Allocator> &m2
)
{
	m1.swap(m2);
}

//  28.10.8, match_results comparisons
template <class BidirectionalIterator, class Allocator>
bool operator==(
	const match_results<BidirectionalIterator, Allocator> &m1,
	const match_results<BidirectionalIterator, Allocator> &m2
)
{
	if (!m1.ready() && !m2.ready())
		return true;

	if (m1.ready() && m2.ready())
	{
		if (m1.empty() && m2.empty())
			return true;

		if (!m1.empty() && !m2.empty())
		{
			return m1.prefix() == m2.prefix() && m1.size() == m2.size() && std::equal(m1.begin(), m1.end(), m2.begin()) && m1.suffix() == m2.suffix();
		}
	}
	return false;
}

template <class BidirectionalIterator, class Allocator>
bool operator!=(
	const match_results<BidirectionalIterator, Allocator> &m1,
	const match_results<BidirectionalIterator, Allocator> &m2
)
{
	return !(m1 == m2);
}

typedef match_results<const char *> cmatch;
typedef match_results<const wchar_t *> wcmatch;
typedef match_results<std::string::const_iterator> smatch;
typedef match_results<std::wstring::const_iterator> wsmatch;

#if defined(SRELL_CPP11_CHAR1632_ENABLED)
	typedef match_results<const char16_t *> u16cmatch;
	typedef match_results<const char32_t *> u32cmatch;
	typedef match_results<std::u16string::const_iterator> u16smatch;
	typedef match_results<std::u32string::const_iterator> u32smatch;
#endif

#if defined(CHAR_BIT) && CHAR_BIT >= 8
	typedef cmatch u8cmatch;
	typedef smatch u8smatch;
#endif

#if defined(WCHAR_MAX)
	#if WCHAR_MAX >= 0x10ffff
		typedef wcmatch u32wcmatch;
		typedef wsmatch u32wsmatch;
	#elif WCHAR_MAX >= 0xffff
		typedef wcmatch u16wcmatch;
		typedef wsmatch u16wsmatch;
	#endif
#endif

//  ... "regex_match_results.hpp"]
//  ["rei_algorithm.hpp" ...

	namespace regex_internal
	{

template <typename charT, typename traits>
class regex_object : public re_compiler<charT, traits>
{
public:

	template <typename BidirectionalIterator>
	bool search
	(
		BidirectionalIterator first,
		const BidirectionalIterator &last,
		match_results<BidirectionalIterator> &results,
//		const basic_regex<charT> &expression,
		const regex_constants::match_flag_type &flags /* = regex_constants::match_default */
	) const
	{
		return !this->is_icase() ? do_search<canonicaliser<uchar21_t, traits, false> >(first, last, results, flags) : do_search<canonicaliser<uchar21_t, traits, true> >(first, last, results, flags);
	}

private:

	template <typename canonicaliser_type, typename BidirectionalIterator>
	bool do_search
	(
		BidirectionalIterator &first,
		const BidirectionalIterator &last,
		match_results<BidirectionalIterator> &results,
		const regex_constants::match_flag_type &flags
	) const
	{
		if (this->NFA_states.size())
		{
			const BidirectionalIterator searchend = (flags & regex_constants::match_continuous) ? first : last;

			results.clear_();
			results.state_.init(first, last, flags, this->NFA_states[0].next_state1, this->number_of_brackets, this->number_of_counters, this->number_of_repeats);

			while (results.state_.nextpos != searchend)
			{
				first = results.state_.nextpos;

#ifdef SRELLDBG_NO_1STCHRCLS
				this->traits_inst.codepoint_inc(results.state_.nextpos, last);
#else
				{
					const uchar21_t firstchar = this->traits_inst.codepoint_inc(results.state_.nextpos, last);

  #if !defined(SRELLDBG_NO_BITSET)
					if (firstchar > constants::unicode_max_codepoint || !this->firstchar_class_bs[firstchar])
  #else
					if (!this->firstchar_class.is_included(!this->is_icase() ? firstchar : this->traits_inst.unicode_casefolding(firstchar)))
  #endif
						continue;
				}
#endif

				FINAL_TRY:
#if defined(SRELL_NO_LIMIT_COUNTER)
				results.state_.reset(first);
#else
				results.state_.reset(first, this->limit_counter);
#endif

				if (run_automaton<canonicaliser_type>(results.state_))
					return results.set_match_results_();
			}

			//  We are here because (flags & regex_constants::match_continuous) == true
			//  or first == last. Even in the latter case, we have to try once more
			//  for such expressions as "" =~ /^$/ or "..." =~ /$/.

			if (!results.state_.is_last_tried())
			{
				first = results.state_.nextpos;
				goto FINAL_TRY;
			}

			return results.mark_as_failed_();
				//  28.11.2: Postconditions: m.ready() == true in all cases.
				//  28.11.3: Postconditions: m.ready() == true in all cases.
		}
		return results.mark_as_failed_();
	}

private:

	template <typename T, typename TraitsT, const bool>
	struct canonicaliser
	{
		static T canonicalise(const T &t, const TraitsT & /* traits_inst */)
		{
			return t;
		}
	};

	template <typename T, typename TraitsT>
	struct canonicaliser<T, TraitsT, true>
	{
		static T canonicalise(const T &t, const TraitsT &traits_inst)
		{
			return traits_inst.unicode_casefolding(t);
		}
	};

	template <typename canonicaliser_type, typename BidirectionalIterator>
	bool run_automaton
	(
//		match_results<BidirectionalIterator> &results,
//		const basic_regex<charT> &expression,
		re_search_state</*charT, */BidirectionalIterator> &state
#if !defined(SRELL_NO_RECURSIVE)
		, const bool is_recursive = false
#endif
	) const
	{
		typedef typename re_object_core<charT, traits>::state_type state_type;
		typedef re_search_state</*charT, */BidirectionalIterator> ss_type;
		bool is_matched;

		for (;;)
		{
			const state_type &current_NFA = *state.nth.in_NFA_states;

			switch (current_NFA.type)
			{
			case st_character:

				if (!state.is_last())
				{
#if !defined(SRELLDBG_NO_ASTERISK_OPT)
					const BidirectionalIterator prevpos = state.nth.in_string;
#endif
					const uchar21_t uchar = this->traits_inst.codepoint_inc(state.nth.in_string, state.last);

					if (current_NFA.character == canonicaliser_type::canonicalise(uchar, this->traits_inst))
						goto MATCHED;

#if !defined(SRELLDBG_NO_ASTERISK_OPT)
					if (current_NFA.next_state2)
					{
						state.nth.in_string = prevpos;
						state.nth.in_NFA_states = current_NFA.next_state2;
						continue;
					}
#endif
				}
				goto NOT_MATCHED;

			case st_character_class:

				if (!state.is_last())
				{
#if !defined(SRELLDBG_NO_ASTERISK_OPT)
					const BidirectionalIterator prevpos = state.nth.in_string;
#endif
					const uchar21_t uchar = this->traits_inst.codepoint_inc(state.nth.in_string, state.last);

					if (current_NFA.is_not ^ this->character_class.is_included(current_NFA.number, canonicaliser_type::canonicalise(uchar, this->traits_inst)))
						goto MATCHED;

#if !defined(SRELLDBG_NO_ASTERISK_OPT)
					if (current_NFA.next_state2)
					{
						state.nth.in_string = prevpos;
						state.nth.in_NFA_states = current_NFA.next_state2;
						continue;
					}
#endif
				}
				goto NOT_MATCHED;

			case st_epsilon:

#if defined(SRELLDBG_NOT_SKIP_EPSILON)
				if (current_NFA.next_state2)
#endif
				{

					state.bt_stack.push_back(state.nth);	//	state.push();
				}

				state.nth.in_NFA_states = current_NFA.next_state1;
				continue;

			default:
				switch (current_NFA.type)
				{

			case st_check_counter:
				{
					const unsigned int &counter = state.counter[current_NFA.number];

					if (counter < current_NFA.quantifier.atmost || current_NFA.quantifier.is_infinity())
					{
						if (counter >= current_NFA.quantifier.atleast)
						{
#if !defined(SRELLDBG_NO_ASTERISK_OPT)
							if (!current_NFA.dont_push)
#endif
								state.bt_stack.push_back(state.nth);
							state.nth.in_NFA_states = current_NFA.next_state1;
						}
						else
						{
							state.nth.in_NFA_states
								= current_NFA.quantifier.is_greedy
								? current_NFA.next_state1
								: current_NFA.next_state2;
						}
					}
					else
					{
						state.nth.in_NFA_states
							= current_NFA.quantifier.is_greedy
							? current_NFA.next_state2
							: current_NFA.next_state1;
					}
				}
				continue;

			case st_increment_counter:
				state.bt_stack.push_back(state.nth);
				++state.counter[current_NFA.number];
				goto MATCHED;

			case st_decrement_counter:
				--state.counter[current_NFA.number];
				goto NOT_MATCHED;

			case st_save_and_reset_counter:
				{
					typename ss_type::counter_type &c = state.counter[current_NFA.number];

#if defined(SRELLTEST_PUSHPOP_OPT)
					if (!current_NFA.dont_push)
#endif
					{
						state.counter_stack.push_back(c);
						state.bt_stack.push_back(state.nth);
					}
					c = 0;
				}
				goto MATCHED;

			case st_restore_counter:
//				if (!current_NFA.dont_push)
				{
					state.counter[current_NFA.number] = state.counter_stack.back();
					state.counter_stack.pop_back();
				}
				goto NOT_MATCHED;

			case st_roundbracket_open:	//  '(':
				{
					typename ss_type::submatch_type &bracket = state.bracket[current_NFA.number];

#if defined(SRELLTEST_PUSHPOP_OPT)
					if (!current_NFA.dont_push)
#endif
					{
						state.capture_stack.push_back(bracket.core);

						for (unsigned int brno = current_NFA.quantifier.atleast + 1; brno <= current_NFA.quantifier.atmost; ++brno)
						{
							typename ss_type::submatch_type &inner_bracket = state.bracket[brno];

							state.capture_stack.push_back(inner_bracket.core);
							state.counter_stack.push_back(inner_bracket.counter);
							inner_bracket.core.open_at = inner_bracket.core.close_at = state.last;
							inner_bracket.counter = 0;
							//  ECMAScript spec 15.10.2.5, note 3.
						}
					}

					bracket.core.open_at = state.nth.in_string;
					++bracket.counter;

					state.bt_stack.push_back(state.nth);
				}
				goto MATCHED;

			case st_roundbracket_pop:	//  '/':
				{
					typename ss_type::submatch_type &bracket = state.bracket[current_NFA.number];

#if defined(SRELLTEST_PUSHPOP_OPT)
					if (!current_NFA.dont_push)
#endif
					{
						for (unsigned int brno = current_NFA.quantifier.atmost; brno > current_NFA.quantifier.atleast; --brno)
						{
							typename ss_type::submatch_type &inner_bracket = state.bracket[brno];

							inner_bracket.counter = state.counter_stack.back();
							inner_bracket.core = state.capture_stack.back();
							state.counter_stack.pop_back();
							state.capture_stack.pop_back();
						}

						bracket.core = state.capture_stack.back();
						state.capture_stack.pop_back();
					}
					--bracket.counter;
				}
				goto NOT_MATCHED;

			case st_roundbracket_close:	//  ')':
				{
					typename ss_type::submatch_type &bracket = state.bracket[current_NFA.number];
					typename ss_type::submatch_core &brc = bracket.core;

					if (brc.open_at != state.nth.in_string)
					{
						state.nth.in_NFA_states = current_NFA.next_state1;
					}
					else	//  0 width match, breaks from the loop.
					{
						if (current_NFA.next_state1->type != st_check_counter)
						{
							if (bracket.counter > 1)
								goto NOT_MATCHED;	//  ECMAScript spec 15.10.2.5, note 4.

							state.nth.in_NFA_states = current_NFA.next_state2;
								//  Accepts 0 width match and exits.
						}
						else
						{
							//  A pair with check_counter.
							const typename ss_type::counter_type &counter = state.counter[current_NFA.next_state1->number];

							if (counter > current_NFA.quantifier.atleast)
								goto NOT_MATCHED;	//  Takes a captured string in the previous loop.

							state.nth.in_NFA_states = current_NFA.next_state1;
								//  Accepts 0 width match and continues.
						}
					}
					brc.close_at = state.nth.in_string;
				}
				continue;

			case st_repeat_in_push:
				{
					typename ss_type::position_type &r = state.repeat[current_NFA.number];

					state.repeat_stack.push_back(r);
					state.bt_stack.push_back(state.nth);
					r = state.nth.in_string;
				}
				goto MATCHED;

			case st_repeat_in_pop:
				state.repeat[current_NFA.number] = state.repeat_stack.back();
				state.repeat_stack.pop_back();
				goto NOT_MATCHED;

			case st_check_0_width_repeat:
				if (state.nth.in_string != state.repeat[current_NFA.number])
					goto MATCHED;

				state.nth.in_NFA_states = current_NFA.next_state2;
				continue;

			case st_backreference:	//  '\\':
				{
					const typename ss_type::submatch_type &bracket = state.bracket[current_NFA.number];

					if (!bracket.counter)	//  Undefined.
					{
						ESCAPE_FROM_ZERO_WIDTH_MATCH:
						state.nth.in_NFA_states = current_NFA.next_state2;
						continue;
					}
					else
					{
						const typename ss_type::submatch_core &brc = bracket.core;

						if (brc.open_at == brc.close_at)
						{
							goto ESCAPE_FROM_ZERO_WIDTH_MATCH;
						}
						else
						{
							for (BidirectionalIterator backrefpos = brc.open_at; backrefpos != brc.close_at;)
							{
								if (!state.is_last())
								{
									const uchar21_t uchartxt = this->traits_inst.codepoint_inc(state.nth.in_string, state.last);
									const uchar21_t ucharref = this->traits_inst.codepoint_inc(backrefpos, brc.close_at);

									if (canonicaliser_type::canonicalise(uchartxt, this->traits_inst) == canonicaliser_type::canonicalise(ucharref, this->traits_inst))
										continue;
								}
								goto NOT_MATCHED;
							}
						}
					}
				}
				goto MATCHED;

			case st_lookaround_open:
				{
					for (unsigned int i = 1; i < this->number_of_brackets; ++i)
					{
						const typename ss_type::submatch_type &sm = state.bracket[i];
						state.capture_stack.push_back(sm.core);
						state.counter_stack.push_back(sm.counter);
					}

					for (unsigned int i = 0; i < this->number_of_counters; ++i)
						state.counter_stack.push_back(state.counter[i]);

					for (unsigned int i = 0; i < this->number_of_repeats; ++i)
						state.repeat_stack.push_back(state.repeat[i]);

#if !defined(SRELL_NO_RECURSIVE)

					const typename ss_type::bottom_state backup_bottom(state.btstack_size, state.capture_stack.size(), state.counter_stack.size(), state.repeat_stack.size());

#else
					state.stacks_stack.push_back(typename ss_type::bottom_state(state.btstack_size, state.capture_stack.size(), state.counter_stack.size(), state.repeat_stack.size()));
#endif

					state.bt_stack.push_back(state.nth);
					state.btstack_size = state.bt_stack.size();

#if !defined(SRELL_NO_REVERSE)

//					if (current_NFA.reverse)
					{
						for (unsigned int i = 0; i < current_NFA.quantifier.atleast; ++i)
						{
							if (!state.is_base())
							{
								this->traits_inst.dec_codepoint(state.nth.in_string, state.base);
								continue;
							}
							is_matched = false;
							goto AFTER_LOOKAROUND;
						}
					}

#endif	//  !defined(SRELL_NO_REVERSE)

					++state.nth.in_NFA_states;	//  Moves to lookaround_pop,
					state.nth.in_NFA_states = state.nth.in_NFA_states->next_state1;
						//  Due to shortage of "next_state"s of lookaround_open!

#if !defined(SRELL_NO_RECURSIVE)
					is_matched = run_automaton<canonicaliser_type>(/* results, */ state, true);
#else
					continue;
#endif

					AFTER_LOOKAROUND:	//  DO NOT USE current_NFA IN THIS BLOCK.
					{
						const typename ss_type::search_core_state &lookaround_open_pair = state.bt_stack[state.btstack_size - 1];

						state.nth.in_NFA_states = lookaround_open_pair.in_NFA_states;

#if defined(SRELL_ENABLE_GT)
						if (state.nth.in_NFA_states->character != meta_char::mc_gt)	//  '>'
#endif
						{
							state.nth.in_string = lookaround_open_pair.in_string;
						}
						state.bt_stack.resize(state.btstack_size);

#if defined(SRELL_NO_RECURSIVE)
						const typename ss_type::bottom_state &backup_bottom = state.stacks_stack.back();
#endif

						state.btstack_size = backup_bottom.btstack_size;
						state.capture_stack.resize(backup_bottom.capturestack_size);
						state.counter_stack.resize(backup_bottom.counterstack_size);
						state.repeat_stack.resize(backup_bottom.repeatstack_size);

#if defined(SRELL_NO_RECURSIVE)
						state.stacks_stack.pop_back();
#endif
						is_matched ^= state.nth.in_NFA_states->is_not;
					}
				}
				break;

			case st_lookaround_pop:
				for (unsigned int i = this->number_of_repeats; i;)
				{
					state.repeat[--i] = state.repeat_stack.back();
					state.repeat_stack.pop_back();
				}

				for (unsigned int i = this->number_of_counters; i;)
				{
					state.counter[--i] = state.counter_stack.back();
					state.counter_stack.pop_back();
				}

				for (unsigned int i = this->number_of_brackets; i > 1;)
				{
					typename ss_type::submatch_type &sm = state.bracket[--i];

					sm.counter = state.counter_stack.back();
					sm.core = state.capture_stack.back();
					state.counter_stack.pop_back();
					state.capture_stack.pop_back();
				}
				goto NOT_MATCHED;

			case st_bol:	//  '^':
				if (state.is_base() && !state.match_prev_avail_flag())
				{
					if (!state.match_not_bol_flag())
						goto MATCHED;
				}
					//  !state.is_base() || state.match_prev_avail_flag()
				else if (this->is_multiline())
				{
					const uchar21_t prevchar = this->traits_inst.prevcodepoint(state.nth.in_string);

					if (this->character_class.is_newline(prevchar))
						goto MATCHED;
				}
				goto NOT_MATCHED;

			case st_eol:	//  '$':
				if (state.is_last())
				{
					if (!state.match_not_eol_flag())
						goto MATCHED;
				}
				else if (this->is_multiline())
				{
					const uchar21_t nextchar = this->traits_inst.codepoint(state.nth.in_string, state.last);

					if (this->character_class.is_newline(nextchar))
						goto MATCHED;
				}
				goto NOT_MATCHED;

			case st_boundary:	//  '\b' '\B'
				is_matched = current_NFA.is_not;

				//  First, suppose the previous character is not \w.

				if (state.is_last())
				{
					if (state.match_not_eow_flag())
						is_matched = !is_matched;
				}
				else if (this->character_class.is_word(this->traits_inst.codepoint(state.nth.in_string, state.last)))
				{
					is_matched = !is_matched;
				}
				//      \W/last     \w
				//  \b  false       true
				//  \B  true        false

				//  Second, if the actual previous character is \w, inverse is_matched.

				if (state.is_base() && !state.match_prev_avail_flag())
				{
					if (state.match_not_bow_flag())
						is_matched = !is_matched;
				}
					//  !state.is_base() || state.match_prev_avail_flag())
				else if (this->character_class.is_word(this->traits_inst.prevcodepoint(state.nth.in_string)))
				{
					is_matched = !is_matched;
				}
				//  \b                          \B
				//  pre cur \W/last \w          pre cur \W/last \w
				//  \W/base false   true        \W/base true    false
				//  \w      true    false       \w      false   true

				break;

			case st_success:	//  == lookaround_close.
#if !defined(SRELL_NO_RECURSIVE)
				if (is_recursive)
					return true;

				if
				(	//  check if null_match,
					(!state.match_not_null_flag() || !state.is_null())
					&&
					//  check if regex_match.
					(!state.match_match_flag() || state.is_last())
				)
					return true;

				goto NOT_MATCHED;
#else
				if (!state.stacks_stack.size())	//  not recursive.
				{
					if
					(	//  check if null_match,
						(!state.match_not_null_flag() || !state.is_null())
						&&
						//  check if regex_match.
						(!state.match_match_flag() || state.is_last())
					)
						return true;

					goto NOT_MATCHED;
				}

				is_matched = true;
				goto AFTER_LOOKAROUND;
#endif

#if !defined(SRELLDBG_NO_NEXTPOS_OPT)
			case st_move_nextpos:
				if (state.nth.in_string != state.bracket[0].core.open_at)
					state.nextpos = state.nth.in_string;
				goto MATCHED;
#endif

			default:
				throw regex_error(regex_constants::error_internal);

				}
			}

			if (is_matched)
			{
				MATCHED:
//				state.nth.in_NFA_states = current_NFA.next_state1;
				state.nth.in_NFA_states = state.nth.in_NFA_states->next_state1;
			}
			else
			{
				NOT_MATCHED:

#if !defined(SRELL_NO_LIMIT_COUNTER)
				if (--state.failure_counter)
				{
#endif
					if (state.bt_stack.size() > state.btstack_size)
					{
						state.nth = state.bt_stack.back();
						state.bt_stack.pop_back();

						state.nth.in_NFA_states = state.nth.in_NFA_states->next_state2;
//						continue;
					}
					else
					{
#if defined(SRELL_NO_RECURSIVE)
						if (state.stacks_stack.size())
						{
							is_matched = false;
							goto AFTER_LOOKAROUND;
						}
#endif
						return false;
					}
#if !defined(SRELL_NO_LIMIT_COUNTER)
				}
				else
					throw regex_error(regex_constants::error_complexity);
#endif
			}
		}
	}
};
//  regex_object

	}	//  namespace regex_internal

//  ... "rei_algorithm.hpp"]
//  ["basic_regex.hpp" ...

//  28.8, class template basic_regex:
template <class charT, class traits = regex_traits<charT> >
class basic_regex : public regex_internal::regex_object<charT, traits>
{
public:

	//  types:
	typedef charT value_type;
	typedef traits traits_type;
	typedef typename traits::string_type string_type;
	typedef regex_constants::syntax_option_type flag_type;
	typedef typename traits::locale_type locale_type;

	//  28.8.1, constants:
	//  [7.8.1] constants
	static const regex_constants::syntax_option_type icase = regex_constants::icase;
	static const regex_constants::syntax_option_type nosubs = regex_constants::nosubs;
	static const regex_constants::syntax_option_type optimize = regex_constants::optimize;
	static const regex_constants::syntax_option_type collate = regex_constants::collate;
	static const regex_constants::syntax_option_type ECMAScript = regex_constants::ECMAScript;
	static const regex_constants::syntax_option_type basic = regex_constants::basic;
	static const regex_constants::syntax_option_type extended = regex_constants::extended;
	static const regex_constants::syntax_option_type awk = regex_constants::awk;
	static const regex_constants::syntax_option_type grep = regex_constants::grep;
	static const regex_constants::syntax_option_type egrep = regex_constants::egrep;

	static const regex_constants::syntax_option_type multiline = regex_constants::multiline;

	//  28.8.2, construct/copy/destroy:
	//  [7.8.2] construct/copy/destroy
	basic_regex()
	{
	}

	explicit basic_regex(const charT *const p, const flag_type f = regex_constants::ECMAScript)
	{
		assign(p, p + std::char_traits<charT>::length(p), f);
	}

	basic_regex(const charT *const p, const std::size_t len, const flag_type f)
	{
		assign(p, p + len, f);
	}

	basic_regex(const basic_regex &e)
	{
		assign(e);
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	basic_regex(basic_regex &&e) noexcept	//  C++11.
	{
		assign(std::move(e));
	}
#endif

	template <class ST, class SA>
	explicit basic_regex(const std::basic_string<charT, ST, SA> &p, const flag_type f = regex_constants::ECMAScript)
	{
		assign(p.begin(), p.end(), f);
	}

	template <class ForwardIterator>	//  was InputIterator in TR1.
	basic_regex(ForwardIterator first, ForwardIterator last, const flag_type f = regex_constants::ECMAScript)
	{
		assign(first, last, f);
	}

#if defined(SRELL_CPP11_INITIALIZER_LIST_ENABLED)
	basic_regex(std::initializer_list<charT> il, const flag_type f = regex_constants::ECMAScript)	//  C++11.
	{
		assign(il, f);
		//  OK with this?
	}
#endif

//	~basic_regex();

	basic_regex &operator=(const basic_regex &right)
	{
		return assign(right);
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	basic_regex &operator=(basic_regex &&e) noexcept	//  C++11.
	{
		return assign(std::move(e));
	}
#endif

	basic_regex &operator=(const charT *const ptr)
	{
		return assign(ptr);
	}

#if defined(SRELL_CPP11_INITIALIZER_LIST_ENABLED)
	basic_regex &operator=(std::initializer_list<charT> il)	//  C++11.
	{
		return assign(il);
		//  OK with this?
	}
#endif

	template <class ST, class SA>
	basic_regex &operator=(const std::basic_string<charT, ST, SA> &p)
	{
		return assign(p);
	}

	//  28.8.3, assign:
	//  [7.8.3] assign
	basic_regex &assign(const basic_regex &right)
	{
		regex_internal::re_object_core<charT, traits>::operator=(right);
		return *this;
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	basic_regex &assign(basic_regex &&right) noexcept	//  C++11.
	{
		regex_internal::re_object_core<charT, traits>::operator=(std::move(right));
		return *this;
	}
#endif

	basic_regex &assign(const charT *const ptr, const flag_type f = regex_constants::ECMAScript)
	{
		return assign(ptr, ptr + std::char_traits<charT>::length(ptr), f);
	}

	basic_regex &assign(const charT *const p, std::size_t len, const flag_type f)
	{
		return assign(p, p + len, f);
	}

	template <class string_traits, class A>
	basic_regex &assign(const std::basic_string<charT, string_traits, A> &s, const flag_type f = regex_constants::ECMAScript)
	{
		return assign(s.begin(), s.end(), f);
	}

	template <class InputIterator>
	basic_regex &assign(InputIterator first, InputIterator last, const flag_type f = regex_constants::ECMAScript)
	{
		this->compile(first, last, f);
		return *this;
	}

#if defined(SRELL_CPP11_INITIALIZER_LIST_ENABLED)
	basic_regex &assign(std::initializer_list<charT> il, const flag_type f = regex_constants::ECMAScript)	//  C++11.
	{
		return assign(il.begin(), il.end(), f);
		//  OK with this?
	}
#endif

	//  28.8.4, const operations:
	//  [7.8.4] const operations
	unsigned mark_count() const
	{
		return this->number_of_brackets - 1;
	}

	flag_type flags() const
	{
		return this->flags_;
	}

	//  28.8.5, locale:
	//  [7.8.5] locale
	locale_type imbue(locale_type loc)
	{
		return this->traits_inst.imbue(loc);
	}

	locale_type getloc() const
	{
		return this->traits_inst.getloc();
	}

	//  28.8.6, swap:
	//  [7.8.6] swap
	void swap(basic_regex &e)
	{
		basic_regex tmp(e);
		e = *this;
		*this = tmp;
	}
};

//  28.8.6, basic_regex swap:
template <class charT, class traits>
void swap(basic_regex<charT, traits> &lhs, basic_regex<charT, traits> &rhs)
{
	lhs.swap(rhs);
}

typedef basic_regex<char> regex;
typedef basic_regex<wchar_t> wregex;

#if defined(WCHAR_MAX)
	#if WCHAR_MAX >= 0x10ffff
		typedef wregex u32wregex;
	#elif WCHAR_MAX >= 0xffff
		typedef basic_regex<wchar_t, u16regex_traits<wchar_t> > u16wregex;
	#endif
#endif

#if defined(CHAR_BIT) && CHAR_BIT >= 8
	typedef basic_regex<char, u8regex_traits<char> > u8regex;
#endif

#if defined(SRELL_CPP11_CHAR1632_ENABLED)
	typedef basic_regex<char16_t, u16regex_traits<char16_t> > u16regex;
	typedef basic_regex<char32_t> u32regex;
#endif

//  ... "basic_regex.hpp"]
//  ["regex_iterator.hpp" ...

//  28.12.1, class template regex_iterator:
template <class BidirectionalIterator, class charT = typename std::iterator_traits<BidirectionalIterator>::value_type, class traits = regex_traits<charT> >
class regex_iterator
{
public:

	typedef basic_regex<charT, traits> regex_type;
	typedef match_results<BidirectionalIterator> value_type;
	typedef std::ptrdiff_t difference_type;
	typedef const value_type * pointer;
	typedef const value_type & reference;
	typedef std::forward_iterator_tag iterator_category;

	regex_iterator()
	{
		//  28.12.1.1: Constructs an end-of-sequence iterator.
	}

	regex_iterator(
		const BidirectionalIterator a,
		const BidirectionalIterator b,
		const regex_type &re,
		const regex_constants::match_flag_type m = regex_constants::match_default)
		: begin(a), end(b), pregex(&re), flags(m)
	{
		regex_search(begin, end, match, *pregex, flags);
			//  28.12.1.1: If this call returns false the constructor
			//    sets *this to the end-of-sequence iterator.
	}

	regex_iterator(const regex_iterator &that)
	{
		operator=(that);
	}

	regex_iterator &operator=(const regex_iterator &that)
	{
		if (this != &that)
		{
			this->begin = that.begin;
			this->end = that.end;
			this->pregex = that.pregex;
			this->flags = that.flags;
			this->match = that.match;
			this->traits_inst_ = that.traits_inst_;
		}
		return *this;
	}

	bool operator==(const regex_iterator &right) const
	{
		//  It is probably safe to assume that match.size() == 0 means
		//  end-of-sequence, because it happens only when 1) never tried
		//  regex_search, or 2) regex_search returned false.

		if (this->match.size() == 0 || right.match.size() == 0)
			return this->match.size() == right.match.size();

		return
			this->begin == right.begin
			&&
			this->end == right.end
			&&
			this->pregex == right.pregex
			&&
			this->flags == right.flags
			&&
			this->match[0] == right.match[0];
	}

	bool operator!=(const regex_iterator &right) const
	{
		return !(*this == right);
	}

	const value_type &operator*() const
	{
		return match;
	}

	const value_type *operator->() const
	{
		return &match;
	}

	regex_iterator &operator++()
	{

		if (this->match.size())
		{
			BidirectionalIterator start = match[0].second;

			if (match[0].first == start)	//  The iterator holds a 0-length match.
			{
				if (start == end)
				{
					match.clear_();
					return *this;
					//    28.12.1.4.2: If the iterator holds a zero-length match and
					//  start == end the operator sets *this to the end-ofsequence
					//  iterator and returns *this.
				}

				//    28.12.1.4.3: Otherwise, if the iterator holds a zero-length match
				//  the operator calls regex_search(start, end, match, *pregex, flags
				//  | regex_constants::match_not_null | regex_constants::match_continuous).
				//  If the call returns true the operator returns *this. [Cont...]

				if (!regex_search(start, end, match, *pregex, flags | regex_constants::match_not_null | regex_constants::match_continuous))
				{
					//  [...Cont] Otherwise the operator increments start and continues
					//  as if the most recent match was not a zero-length match.
//					++start;
					traits_inst_.codepoint_inc(start, end);
					goto ASIF_NOT_NULLMATCH;
				}
			}
			else
			{
				ASIF_NOT_NULLMATCH:
				//    28.12.1.4.4: If the most recent match was not a zero-length match,
				//  the operator sets flags to flags | regex_constants::match_prev_avail
				//  and calls regex_search(start, end, match, *pregex, flags). [Cont...]

				flags |= regex_constants::match_prev_avail;

				if (!regex_search(start, end, match, *pregex, flags))
				{
//					match.clear_();	//  Should be done in regex_search.
					return *this;
					//  [...Cont] If the call returns false the iterator sets *this to
					//  the end-of-sequence iterator. The iterator then returns *this.
				}
			}

			//  We are here since operator succeeded.

			//    28.12.1.4.5: In all cases in which the call to regex_search
			//  returns true, match.prefix().first shall be equal to the previous
			//  value of match[0].second, ...
			//  This condition is automatically satisfied by passing 'start' as
			//  a 'begin' argument to regex_search.

			//  28.12.1.4.5: ... match[i].position() shall return distance(begin, match[i].first).
			//  To satisfy this:
			match.set_base_(begin);
		}
		return *this;
	}

	regex_iterator operator++(int)
	{
		const regex_iterator tmp = *this;
		++(*this);
		return tmp;
	}

private:

	BidirectionalIterator                begin;
	BidirectionalIterator                end;
	const regex_type                    *pregex;
	regex_constants::match_flag_type     flags;
	match_results<BidirectionalIterator> match;

	typedef traits traits_type;
	traits_type traits_inst_;
};

typedef regex_iterator<const char *> cregex_iterator;
typedef regex_iterator<const wchar_t *> wcregex_iterator;
typedef regex_iterator<std::string::const_iterator> sregex_iterator;
typedef regex_iterator<std::wstring::const_iterator> wsregex_iterator;

#if defined(SRELL_CPP11_CHAR1632_ENABLED)
	typedef regex_iterator<const char16_t *> u16cregex_iterator;
	typedef regex_iterator<const char32_t *> u32cregex_iterator;
	typedef regex_iterator<std::u16string::const_iterator> u16sregex_iterator;
	typedef regex_iterator<std::u32string::const_iterator> u32sregex_iterator;
#endif

#if defined(CHAR_BIT) && CHAR_BIT >= 8
	typedef regex_iterator<const char *, typename std::iterator_traits<const char *>::value_type, u8regex_traits<typename std::iterator_traits<const char *>::value_type> > u8cregex_iterator;
	typedef regex_iterator<std::string::const_iterator, typename std::iterator_traits<std::string::const_iterator>::value_type, u8regex_traits<typename std::iterator_traits<std::string::const_iterator>::value_type> > u8sregex_iterator;
#endif

#if defined(WCHAR_MAX)
	#if WCHAR_MAX >= 0x10ffff
		typedef wcregex_iterator u32wcregex_iterator;
		typedef wsregex_iterator u32wsregex_iterator;
	#elif WCHAR_MAX >= 0xffff
		typedef regex_iterator<const wchar_t *, typename std::iterator_traits<const wchar_t *>::value_type, u16regex_traits<typename std::iterator_traits<const wchar_t *>::value_type> > u16wcregex_iterator;
		typedef regex_iterator<std::wstring::const_iterator, typename std::iterator_traits<std::wstring::const_iterator>::value_type, u16regex_traits<typename std::iterator_traits<std::wstring::const_iterator>::value_type> > u16wsregex_iterator;
	#endif
#endif

//  ... "regex_iterator.hpp"]
//  ["regex_algorithm.hpp" ...

//  28.11.2, function template regex_match:
//  [7.11.2] Function template regex_match
template <class BidirectionalIterator, class Allocator, class charT, class traits>
bool regex_match(
	const BidirectionalIterator first,
	const BidirectionalIterator last,
	match_results<BidirectionalIterator, Allocator> &m,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return e.search(first, last, m, flags | regex_constants::match_continuous | regex_constants::match_match_);
}

template <class BidirectionalIterator, class charT, class traits>
bool regex_match(
	const BidirectionalIterator first,
	const BidirectionalIterator last,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
//  4 Effects: Behaves "as if" by constructing an instance of
//  match_results<BidirectionalIterator> what, and then returning the
//  result of regex_match(first, last, what, e, flags).

	match_results<BidirectionalIterator> what;

	return regex_match(first, last, what, e, flags);
}

template <class charT, class Allocator, class traits>
bool regex_match(
	const charT *const str,
	match_results<const charT *, Allocator> &m,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return regex_match(str, str + std::char_traits<charT>::length(str), m, e, flags);
}

template <class ST, class SA, class Allocator, class charT, class traits>
bool regex_match(
	const std::basic_string<charT, ST, SA> &s,
	match_results<typename std::basic_string<charT, ST, SA>::const_iterator, Allocator> &m,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return regex_match(s.begin(), s.end(), m, e, flags);
}

template <class charT, class traits>
bool regex_match(
	const charT *const str,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return regex_match(str, str + std::char_traits<charT>::length(str), e, flags);
}

template <class ST, class SA, class charT, class traits>
bool regex_match(
	const std::basic_string<charT, ST, SA> &s,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return regex_match(s.begin(), s.end(), e, flags);
}

//  28.11.3, function template regex_search:
//  7.11.3 regex_search [tr.re.alg.search]
template <class BidirectionalIterator, class Allocator, class charT, class traits>
bool regex_search(
	const BidirectionalIterator first,
	const BidirectionalIterator last,
	match_results<BidirectionalIterator, Allocator> &m,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return e.search(first, last, m, flags);
}

template <class BidirectionalIterator, class charT, class traits>
bool regex_search(
	const BidirectionalIterator first,
	const BidirectionalIterator last,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
//  6 Effects: Behaves "as if" by constructing an object what of type
//  match_results<iterator> and then returning the result of
//  regex_search(first, last, what, e, flags).

	match_results<BidirectionalIterator> what;
	return regex_search(first, last, what, e, flags);
}

template <class charT, class Allocator, class traits>
bool regex_search(
	const charT *const str,
	match_results<const charT *, Allocator> &m,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return regex_search(str, str + std::char_traits<charT>::length(str), m, e, flags);
}

template <class charT, class traits>
bool regex_search(
	const charT *const str,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return regex_search(str, str + std::char_traits<charT>::length(str), e, flags);
}

template <class ST, class SA, class charT, class traits>
bool regex_search(
	const std::basic_string<charT, ST, SA> &s,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return regex_search(s.begin(), s.end(), e, flags);
}

template <class ST, class SA, class Allocator, class charT, class traits>
bool regex_search(
	const std::basic_string<charT, ST, SA> &s,
	match_results<typename std::basic_string<charT, ST, SA>::const_iterator, Allocator> &m,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return regex_search(s.begin(), s.end(), m, e, flags);
}

//  28.11.4, function template regex_replace:
//  [7.11.4] Function template regex_replace
template <class OutputIterator, class BidirectionalIterator, class traits, class charT, class ST, class SA>
OutputIterator regex_replace(
	OutputIterator out,
	const BidirectionalIterator first,
	const BidirectionalIterator last,
	const basic_regex<charT, traits> &e,
	const std::basic_string<charT, ST, SA> &fmt,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	typedef regex_iterator<BidirectionalIterator, charT, traits> iterator_type;

	const bool do_copy = !(flags & regex_constants::format_no_copy);
	const iterator_type eos;
	iterator_type i(first, last, e, flags);
	typename iterator_type::value_type::value_type last_m_suffix;

	last_m_suffix.first = first;
	last_m_suffix.second = last;

	for (; i != eos; ++i)
	{
		if (do_copy)
			out = std::copy(i->prefix().first, i->prefix().second, out);

		out = i->format(out, fmt, flags);
		last_m_suffix = i->suffix();

		if (flags & regex_constants::format_first_only)
			break;
	}

	if (do_copy)
		out = std::copy(last_m_suffix.first, last_m_suffix.second, out);

	return out;
}

template <class OutputIterator, class BidirectionalIterator, class traits, class charT>
OutputIterator regex_replace(
	OutputIterator out,
	const BidirectionalIterator first,
	const BidirectionalIterator last,
	const basic_regex<charT, traits> &e,
	const charT *const fmt,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	//  Strictly speaking, this should be implemented as a version different
	//  from the above with changing the line i->format(out, fmt, flags) to
	//  i->format(out, fmt, fmt + char_traits<charT>::length(fmt), flags).

	const std::basic_string<charT> fs(fmt, fmt + std::char_traits<charT>::length(fmt));

	return regex_replace(out, first, last, e, fs, flags);
}

template <class traits, class charT, class ST, class SA, class FST, class FSA>
std::basic_string<charT, ST, SA> regex_replace(
	const std::basic_string<charT, ST, SA> &s,
	const basic_regex<charT, traits> &e,
	const std::basic_string<charT, FST, FSA> &fmt,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	std::basic_string<charT, ST, SA> result;

	regex_replace(std::back_inserter(result), s.begin(), s.end(), e, fmt, flags);
	return result;
}

template <class traits, class charT, class ST, class SA>
std::basic_string<charT, ST, SA> regex_replace(
	const std::basic_string<charT, ST, SA> &s,
	const basic_regex<charT, traits> &e,
	const charT *const fmt,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	std::basic_string<charT, ST, SA> result;

	regex_replace(std::back_inserter(result), s.begin(), s.end(), e, fmt, flags);
	return result;
}

template <class traits, class charT, class ST, class SA>
std::basic_string<charT> regex_replace(
	const charT *const s,
	const basic_regex<charT, traits> &e,
	const std::basic_string<charT, ST, SA> &fmt,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	std::basic_string<charT> result;

	regex_replace(std::back_inserter(result), s, s + std::char_traits<charT>::length(s), e, fmt, flags);
	return result;
}

template <class traits, class charT>
std::basic_string<charT> regex_replace(
	const charT *const s,
	const basic_regex<charT, traits> &e,
	const charT *const fmt,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	std::basic_string<charT> result;

	regex_replace(std::back_inserter(result), s, s + std::char_traits<charT>::length(s), e, fmt, flags);
	return result;
}

//  ... "regex_algorithm.hpp"]
//  ["regex_token_iterator.hpp" ...

//  28.12.2, class template regex_token_iterator:
template <class BidirectionalIterator, class charT = typename std::iterator_traits<BidirectionalIterator>::value_type, class traits = regex_traits<charT> >
class regex_token_iterator
{
public:

	typedef basic_regex<charT, traits> regex_type;
	typedef sub_match<BidirectionalIterator> value_type;
	typedef std::ptrdiff_t difference_type;
	typedef const value_type * pointer;
	typedef const value_type & reference;
	typedef std::forward_iterator_tag iterator_category;

	regex_token_iterator() : result(NULL)
	{
		//  Constructs the end-of-sequence iterator.
	}

	regex_token_iterator(
		const BidirectionalIterator a,
		const BidirectionalIterator b,
		const regex_type &re,
		int submatch = 0,
		regex_constants::match_flag_type m = regex_constants::match_default
	) : position(a, b, re, m), result(NULL), subs(1, submatch)
	{
		post_constructor(a, b);
	}

	regex_token_iterator(
		const BidirectionalIterator a,
		const BidirectionalIterator b,
		const regex_type &re,
		const std::vector<int> &submatches,
		regex_constants::match_flag_type m = regex_constants::match_default
	) : position(a, b, re, m), result(NULL), subs(submatches)
	{
		post_constructor(a, b);
	}

#if defined(SRELL_CPP11_INITIALIZER_LIST_ENABLED)
	//  C++11.
	//  OK with this?
	regex_token_iterator(
		const BidirectionalIterator a,
		const BidirectionalIterator b,
		const regex_type &re,
		std::initializer_list<int> submatches,
		regex_constants::match_flag_type m = regex_constants::match_default
	) : position(a, b, re, m), result(NULL), subs(submatches)
	{
		post_constructor(a, b);
	}
#endif

	template <std::size_t N>	//  was R in TR1.
	regex_token_iterator(
		const BidirectionalIterator a,
		const BidirectionalIterator b,
		const regex_type &re,
		const int (&submatches)[N],
		regex_constants::match_flag_type m = regex_constants::match_default
	) : position(a, b, re, m), result(NULL), subs(submatches, submatches + N)
	{
		post_constructor(a, b);
	}

	regex_token_iterator(const regex_token_iterator &that)
	{
		operator=(that);
	}

	regex_token_iterator &operator=(const regex_token_iterator &that)
	{
		if (this != &that)
		{
			this->position = that.position;
			this->result = that.result;
			this->suffix = that.suffix;
			this->N = that.N;
			this->subs = that.subs;
		}
		return *this;
	}

	bool operator==(const regex_token_iterator &right)
	{
		if (this->result == NULL || right.result == NULL)
			return this->result == right.result;

		if (this->result == &this->suffix || right.result == &right.suffix)
			return this->suffix == right.suffix;

		return
			this->position == right.position
			&&
			this->N == right.N
			&&
			this->subs == right.subs;
	}

	bool operator!=(const regex_token_iterator &right)
	{
		return !(*this == right);
	}

	const value_type &operator*()
	{
		return *result;
	}

	const value_type *operator->()
	{
		return result;
	}

	regex_token_iterator &operator++()
	{
		position_iterator prev(position);
		position_iterator eos_iterator;

		if (result != NULL)
			//  To avoid inifinite loop. The specification does not require, though.
		{
			if (result == &suffix)
			{
				result = NULL;	//  end-of-sequence.
			}
			else
			{
				++this->N;
				for (;;)
				{
					if (this->N < subs.size())
					{
						result = subs[this->N] != -1 ? &((*position)[subs[this->N]]) : &((*position).prefix());
						break;
					}

					this->N = 0;
					++position;

					if (position == eos_iterator)
					{
						if (this->N < subs.size() && prev->suffix().length() && minus1_in_subs())
						{
							suffix = prev->suffix();
							result = &suffix;
						}
						else
						{
							result = NULL;
						}
						break;
					}
				}
			}
		}
		return *this;
	}

	regex_token_iterator operator++(int)
	{
		const regex_token_iterator tmp(*this);
		++(*this);
		return tmp;
	}

private:

	void post_constructor(const BidirectionalIterator a, const BidirectionalIterator b)
	{
		position_iterator eos_iterator;

		this->N = 0;

		if (position != eos_iterator && subs.size())
		{
			result = subs[this->N] != -1 ? &((*position)[subs[this->N]]) : &((*position).prefix());
		}
		else if (minus1_in_subs())	//  end-of-sequence.
		{
			suffix.first   = a;
			suffix.second  = b;
			suffix.matched = a != b;
			//  28.1.2.7: In a suffix iterator the member result holds a pointer
			//  to the data member suffix, the value of the member suffix.match is true,

			if (suffix.matched)
				result = &suffix;
			else
				result = NULL;	//  which means end-of-sequence.
		}
	}

	bool minus1_in_subs() const
	{
		for (std::size_t i = 0; i < subs.size(); ++i)
			if (subs[i] == -1)
				return true;

		return false;
	}

private:

	typedef regex_iterator<BidirectionalIterator, charT, traits> position_iterator;
	position_iterator position;
	const value_type *result;
	value_type suffix;
	std::size_t N;
	std::vector<int> subs;
};

typedef regex_token_iterator<const char *> cregex_token_iterator;
typedef regex_token_iterator<const wchar_t *> wcregex_token_iterator;
typedef regex_token_iterator<std::string::const_iterator> sregex_token_iterator;
typedef regex_token_iterator<std::wstring::const_iterator> wsregex_token_iterator;

#if defined(SRELL_CPP11_CHAR1632_ENABLED)
	typedef regex_token_iterator<const char16_t *> u16cregex_token_iterator;
	typedef regex_token_iterator<const char32_t *> u32cregex_token_iterator;
	typedef regex_token_iterator<std::u16string::const_iterator> u16sregex_token_iterator;
	typedef regex_token_iterator<std::u32string::const_iterator> u32sregex_token_iterator;
#endif

#if defined(CHAR_BIT) && CHAR_BIT >= 8
	typedef regex_token_iterator<const char *, typename std::iterator_traits<const char *>::value_type, u8regex_traits<typename std::iterator_traits<const char *>::value_type> > u8cregex_token_iterator;
	typedef regex_token_iterator<std::string::const_iterator, typename std::iterator_traits<std::string::const_iterator>::value_type, u8regex_traits<typename std::iterator_traits<std::string::const_iterator>::value_type> > u8sregex_token_iterator;
#endif

#if defined(WCHAR_MAX)
	#if WCHAR_MAX >= 0x10ffff
		typedef wcregex_token_iterator u32wcregex_token_iterator;
		typedef wsregex_token_iterator u32wsregex_token_iterator;
	#elif WCHAR_MAX >= 0xffff
		typedef regex_token_iterator<const wchar_t *, typename std::iterator_traits<const wchar_t *>::value_type, u16regex_traits<typename std::iterator_traits<const wchar_t *>::value_type> > u16wcregex_token_iterator;
		typedef regex_token_iterator<std::wstring::const_iterator, typename std::iterator_traits<std::wstring::const_iterator>::value_type, u16regex_traits<typename std::iterator_traits<std::wstring::const_iterator>::value_type> > u16wsregex_token_iterator;
	#endif
#endif

//  ... "regex_token_iterator.hpp"]

}		//  namespace srell

#ifdef SRELL_CPP11_CHAR1632_ENABLED
#undef SRELL_CPP11_CHAR1632_ENABLED
#endif

#ifdef SRELL_CPP11_INITIALIZER_LIST_ENABLED
#undef SRELL_CPP11_INITIALIZER_LIST_ENABLED
#endif

#ifdef SRELL_CPP11_MOVE_ENABLED
#undef SRELL_CPP11_MOVE_ENABLED
#endif

#endif	//  SRELL_REGEX_TEMPLATE_LIBRARY
