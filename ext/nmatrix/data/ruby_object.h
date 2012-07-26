/////////////////////////////////////////////////////////////////////
// = NMatrix
//
// A linear algebra library for scientific computation in Ruby.
// NMatrix is part of SciRuby.
//
// NMatrix was originally inspired by and derived from NArray, by
// Masahiro Tanaka: http://narray.rubyforge.org
//
// == Copyright Information
//
// SciRuby is Copyright (c) 2010 - 2012, Ruby Science Foundation
// NMatrix is Copyright (c) 2012, Ruby Science Foundation
//
// Please see LICENSE.txt for additional copyright notices.
//
// == Contributing
//
// By contributing source code to SciRuby, you agree to be bound by
// our Contributor Agreement:
//
// * https://github.com/SciRuby/sciruby/wiki/Contributor-Agreement
//
// == ruby_object.h
//
// Functions and classes for dealing with Ruby objects.

#ifndef RUBY_OBJECT_H
#define RUBY_OBJECT_H

/*
 * Standard Includes
 */

#include <ruby.h>

#include <type_traits>

/*
 * Project Includes
 */

#include "ruby_constants.h"

/*
 * Macros
 */

#define RUBYVAL_IS_ARRAY(val)			(TYPE(val) == T_ARRAY)
#define RUBYVAL_IS_COMPLEX(val)		(TYPE(val) == T_COMPLEX)
#define RUBYVAL_IS_INTEGER(val)		(FIXNUM_P(val))
#define RUBYVAL_IS_FLOAT(val)			(TYPE(val) == T_FLOAT)
#define RUBYVAL_IS_NMATRIX(val) 	(rb_obj_is_kind_of(val, cNMatrix) == Qtrue)
#define RUBYVAL_IS_NUMERIC(val)		(FIXNUM_P(v) or (TYPE(v) == T_FLOAT) or (TYPE(v) == T_COMPLEX) or (TYPE(v) == T_RATIONAL))
#define RUBYVAL_IS_NVECTOR(val) 	(rb_obj_is_kind_of(val, cNVector) == Qtrue)
#define RUBYVAL_IS_RATIONAL(val)	(TYPE(val) == T_RATIONAL)
#define RUBYVAL_IS_STRING(val)    (TYPE(val) == T_STRING)

#define NMATRIX_CHECK_TYPE(val) \
	if (TYPE(val) != T_DATA || (RDATA(val)->dfree != (RUBY_DATA_FUNC)nm_delete && RDATA(val)->dfree != (RUBY_DATA_FUNC)nm_delete_ref)) \
		rb_raise(rb_eTypeError, "Expected NMatrix on left-hand side of operation.");

/*
 * Types
 */

/*
 * Data
 */

/*
 * Classes and Functions
 */

template<typename T, typename U>
struct made_from_same_template : std::false_type {}; 
 
template<template<typename> class Templ, typename Arg1, typename Arg2>
struct made_from_same_template<Templ<Arg1>, Templ<Arg2>> : std::true_type {};

class RubyObject {
	public:
	VALUE rval;
	
	/*
	 * Default constructor.
	 */
	inline RubyObject(VALUE ref) : rval(ref) {}
	
	/*
	 * Copy constructors.
	 */
	inline RubyObject(const RubyObject& other) : rval(other.rval) {}
	
	/*
	 * Binary operator definitions.
	 */
	
	inline RubyObject operator+(const RubyObject& other) const {
		return RubyObject(rb_funcall(this->rval, rbsym_add, 1, other.rval));
	}
	
	inline RubyObject operator-(const RubyObject& other) const {
		return RubyObject(rb_funcall(this->rval, rbsym_sub, 1, other.rval));
	}
	
	inline RubyObject operator*(const RubyObject& other) const {
		return RubyObject(rb_funcall(this->rval, rbsym_mul, 1, other.rval));
	}
	
	inline RubyObject operator/(const RubyObject& other) const {
		return RubyObject(rb_funcall(this->rval, rbsym_div, 1, other.rval));
	}
	
	inline RubyObject operator%(const RubyObject& other) const {
		return RubyObject(rb_funcall(this->rval, rbsym_percent, 1, other.rval));
	}
	
	inline bool operator>(const RubyObject& other) const {
		return rb_funcall(this->rval, rbsym_gt, 1, other.rval) == Qtrue;
	}
	
	inline bool operator<(const RubyObject& other) const {
		return rb_funcall(this->rval, rbsym_lt, 1, other.rval) == Qtrue;
	}
	
	inline bool operator==(const RubyObject& other) const {
		return rb_funcall(this->rval, rbsym_eql, 1, other.rval) == Qtrue;
	}
	
	inline bool operator!=(const RubyObject& other) const {
		return rb_funcall(this->rval, rbsym_neql, 1, other.rval) == Qtrue;
	}
	
	inline bool operator>=(const RubyObject& other) const {
		return rb_funcall(this->rval, rbsym_gte, 1, other.rval) == Qtrue;
	}
	
	inline bool operator<=(const RubyObject& other) const {
		return rb_funcall(this->rval, rbsym_lte, 1, other.rval) == Qtrue;
	}
	
	/*
	 * Convert a Ruby object to an integer.
	 */
	template <typename IntType>
	inline typename std::enable_if<std::is_integral<IntType>::value, IntType>::type to(void) {
		return NUM2INT(this->rval);
	}
	
	/*
	 * Convert a Ruby object to a floating point number.
	 */
	template <typename FloatType>
	inline typename std::enable_if<std::is_floating_point<FloatType>::value, FloatType>::type to(void) {
		return NUM2DBL(this->rval);
	}
	
	/*
	 * Convert a Ruby object to a complex number.
	 */
	template <typename ComplexType>
	inline typename std::enable_if<made_from_same_template<ComplexType, Complex64>::value, ComplexType>::type to(void) {
		if (RUBYVAL_IS_INTEGER(this->rval) or RUBYVAL_IS_FLOAT(this->rval) or RUBYVAL_IS_RATIONAL(this->rval)) {
			return ComplexType(NUM2DBL(this->rval));
			
		} else if (RUBYVAL_IS_COMPLEX(this->rval)) {
			return ComplexType(NUM2DBL(rb_funcall(this->rval, rbsym_real, 0)), NUM2DBL(rb_funcall(this->rval, rbsym_imag, 0)));
			
		} else {
			rb_raise(rb_eTypeError, "Invalid conversion to Complex type.");
		}
	}
	
	/*
	 * Convert a Ruby object to a rational number.
	 */
	template <typename RationalType>
	inline typename std::enable_if<made_from_same_template<RationalType, Rational32>::value, RationalType>::type to(void) {
		if (RUBYVAL_IS_INTEGER(this->rval) or RUBYVAL_IS_FLOAT(this->rval) or RUBYVAL_IS_COMPLEX(this->rval)) {
			return RationalType(NUM2INT(this->rval));
			
		} else if (RUBYVAL_IS_RATIONAL(this->rval)) {
			return RationalType(NUM2INT(rb_funcall(this->rval, rbsym_numer, 0)), NUM2INT(rb_funcall(this->rval, rbsym_denom, 0)));
			
		} else {
			rb_raise(rb_eTypeError, "Invalid conversion to Rational type.");
		}
	}
	
	template <typename OtherType>
	inline operator OtherType () {
		return to<OtherType>();
	}
};

#endif
