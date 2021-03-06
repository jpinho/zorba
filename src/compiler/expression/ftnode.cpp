/*
 * Copyright 2006-2008 The FLWOR Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "stdafx.h"

#include <zorba/error.h>
#include <zorba/internal/cxx_util.h>

#include "diagnostics/xquery_diagnostics.h"
#include "types/casting.h"
#include "util/ascii_util.h"
#include "util/indent.h"
#include "util/locale.h"
#include "util/stl_util.h"

#include "expr_visitor.h"
#include "ftnode.h"
#include "ftnode_visitor.h"

using namespace std;
using namespace zorba::locale;

namespace zorba {

///////////////////////////////////////////////////////////////////////////////


SERIALIZE_INTERNAL_METHOD(ftnode)

SERIALIZE_INTERNAL_METHOD(ftmatch_option)

SERIALIZE_INTERNAL_METHOD(ftprimary)

SERIALIZE_INTERNAL_METHOD(ftpos_filter)

SERIALIZE_INTERNAL_METHOD(ftnode_list)

SERIALIZABLE_CLASS_VERSIONS(ftand)

SERIALIZABLE_CLASS_VERSIONS(ftcontent_filter)

SERIALIZABLE_CLASS_VERSIONS(ftcase_option)

SERIALIZABLE_CLASS_VERSIONS(ftdiacritics_option)

SERIALIZABLE_CLASS_VERSIONS(ftdistance_filter)

SERIALIZABLE_CLASS_VERSIONS(ftextension_option)

SERIALIZABLE_CLASS_VERSIONS(ftextension_selection)

SERIALIZABLE_CLASS_VERSIONS(ftlanguage_option)

SERIALIZABLE_CLASS_VERSIONS(ftmatch_options)

SERIALIZABLE_CLASS_VERSIONS(ftmild_not)

SERIALIZABLE_CLASS_VERSIONS(ftor)

SERIALIZABLE_CLASS_VERSIONS(ftorder_filter)

SERIALIZABLE_CLASS_VERSIONS(ftprimary_with_options)

SERIALIZABLE_CLASS_VERSIONS(ftrange)

SERIALIZABLE_CLASS_VERSIONS(ftscope_filter)

SERIALIZABLE_CLASS_VERSIONS(ftselection)

SERIALIZABLE_CLASS_VERSIONS(ftstem_option)

SERIALIZABLE_CLASS_VERSIONS(ftstop_words)

SERIALIZABLE_CLASS_VERSIONS(ftstop_word_option)

SERIALIZABLE_CLASS_VERSIONS(ftthesaurus_id)

SERIALIZABLE_CLASS_VERSIONS(ftthesaurus_option)

SERIALIZABLE_CLASS_VERSIONS(ftunary_not)

SERIALIZABLE_CLASS_VERSIONS(ftweight)

SERIALIZABLE_CLASS_VERSIONS(ftwild_card_option)

SERIALIZABLE_CLASS_VERSIONS(ftwindow_filter)

SERIALIZABLE_CLASS_VERSIONS(ftwords)

SERIALIZABLE_CLASS_VERSIONS(ftwords_times)


////////// Visit macros ///////////////////////////////////////////////////////

#define DO_CHILDREN( R )  ( !(R & ft_visit_result::no_children) )
#define DO_END_VISIT( R ) ( !(R & ft_visit_result::no_end) )

#define BEGIN_VISIT( V )                                \
  ft_visit_result::type vr0 = (V).begin_visit( *this ); \
  if ( DO_CHILDREN( vr0 ) ) {

#define END_VISIT( V )                                       \
  }                                                          \
  if ( !DO_END_VISIT( vr0 ) ) ; else (V).end_visit( *this ); \
  return vr0

#undef  EV_ACCEPT
#define EV_ACCEPT( EXPR, V )                          \
  {                                                   \
    expr_visitor *const ev = (V).get_expr_visitor();  \
    if ( ev && (EXPR) ) (EXPR)->accept( *ev );        \
  }

#undef  ACCEPT
#define ACCEPT( FTNODE, V ) \
  if ( !(FTNODE) ) ; else   \
  vr0 = (ft_visit_result::type)(vr0 | (FTNODE)->accept( V ))

#undef  ACCEPT_SEQ
#define ACCEPT_SEQ( T, S, V )                 \
  for ( T::const_iterator i = (S).begin();    \
        DO_CHILDREN( vr0 ) && i != (S).end(); \
        ++i )                                 \
    ACCEPT( *i, V );

////////// PUT() macros ///////////////////////////////////////////////////////

#define PUT(OS)                     OS << indent
#undef  BEGIN_PUT
#define BEGIN_PUT(OS,LABEL)         PUT(OS) << #LABEL
#define INDENT_PUT(OS)              OS << " [\n" << inc_indent
#define BEGIN_INDENT_PUT(OS,LABEL)  BEGIN_PUT(OS,LABEL); INDENT_PUT(OS)
#define OUTDENT_PUT(OS)             OS << dec_indent << indent << "]\n"
#undef  END_PUT
#define END_PUT(OS)                 return OS
#define OUTDENT_END_PUT(OS)         OUTDENT_PUT(OS); END_PUT(OS)

#define PUT_LABEL(OS,LABEL) \
  OS << " " #LABEL "="

#define PUT_ATTR_VALUE(OS,LABEL,VALUE) \
  PUT_LABEL(OS,LABEL) << VALUE

#define PUT_ATTR(OS,LABEL) \
  PUT_ATTR_VALUE(OS,LABEL,LABEL##_)

#define PUT_BOOL(OS,LABEL) \
  PUT_ATTR_VALUE( OS, LABEL, (LABEL##_ ? 'T' : 'F') )

#define PUT_EXPR(OS,EXPR) \
  if ( !(EXPR) ) ; else (EXPR)->put(OS)

#define PUT_NODE(OS,NODE) \
  if ( !(NODE) ) ; else (NODE)->put(OS)

#define PUT_SEQ(OS,T,SEQ) \
  FOR_EACH( T, i, SEQ ) (*i)->put( OS )

///////////////////////////////////////////////////////////////////////////////

template<typename PointerType>
inline PointerType clone_ptr(
    PointerType p,
    user_function* udf,
    expr::substitution_t &s )
{
  return static_cast<PointerType>( p->clone( udf, s ).release() );
}


template<>
inline expr* clone_ptr( expr* p, user_function* udf, expr::substitution_t &s ) {
  return static_cast<expr*>( p->clone(udf, s) );
}


template<class RCHandleValueType>
inline RCHandleValueType* clone_ptr( 
    rchandle<RCHandleValueType> const &p,
    user_function* udf,
    expr::substitution_t &s )
{
  return static_cast<RCHandleValueType*>( p->clone(udf, s).release() );
}


template<typename PointerType>
inline PointerType clone_ptr_if(
    PointerType p,
    user_function* udf,
    expr::substitution_t &s )
{
  return p ? clone_ptr( p, udf, s ) : nullptr;
}


template<class RCHandleValueType>
inline RCHandleValueType* clone_ptr_if(
    rchandle<RCHandleValueType> const &p,
    user_function* udf,
    expr::substitution_t &s )
{
  return p.isNull() ? nullptr : clone_ptr( p, udf, s );
}


template<class ContainerType>
void clone_list(
    ContainerType const &from,
    ContainerType *to,
    user_function* udf,
    expr::substitution_t &s )
{
  FOR_EACH( typename ContainerType, i, from )
    to->push_back( clone_ptr( *i, udf, s ) );
}

///////////////////////////////////////////////////////////////////////////////

void ftnode::serialize( serialization::Archiver &ar ) {
  ar & loc_;
}

ftand::ftand( QueryLoc const &loc, ftnode_list_t &list ) :
  ftnode_list( loc, list )
{
}

ft_visit_result::type ftand::accept( ftnode_visitor &v ) {
  BEGIN_VISIT( v );
  ACCEPT_SEQ( ftnode_list_t, get_node_list(), v );
  END_VISIT( v );
}

ftnode_t ftand::clone(user_function* udf, expr::substitution_t &s ) const {
  ftnode_list_t copy;
  clone_list( get_node_list(), &copy, udf, s );
  return new ftand( get_loc(), copy );
}

ostream& ftand::put( ostream &o ) const {
  BEGIN_INDENT_PUT( o, ftand );
  PUT_SEQ( o, ftnode_list_t, get_node_list() );
  OUTDENT_END_PUT( o );
}

void ftand::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftnode_list*)this );
}

ftcase_option::ftcase_option(
  QueryLoc const &loc,
  ft_case_mode::type mode
) :
  ftmatch_option( loc ),
  mode_( mode )
{
}

ft_visit_result::type ftcase_option::accept( ftnode_visitor &v ) {
  BEGIN_VISIT( v );
  END_VISIT( v );
}

ftnode_t ftcase_option::clone(user_function* udf, expr::substitution_t& ) const {
  return new ftcase_option( get_loc(), mode_ );
}

ostream& ftcase_option::put( ostream &o ) const {
  BEGIN_PUT( o, ftcase_option );
  PUT_ATTR( o, mode ) << endl;
  END_PUT( o );
}

void ftcase_option::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftmatch_option*)this );
  SERIALIZE_ENUM(ft_case_mode::type,mode_);
}

ftcontent_filter::ftcontent_filter(
  QueryLoc const &loc, ft_content_mode::type mode
) :
  ftpos_filter( loc ),
  mode_( mode )
{
}

ft_visit_result::type ftcontent_filter::accept( ftnode_visitor &v ) {
  BEGIN_VISIT( v );
  END_VISIT( v );
}

ftnode_t ftcontent_filter::clone(user_function* udf,  expr::substitution_t &s ) const {
  return new ftcontent_filter( get_loc(), mode_ );
}

void ftcontent_filter::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftpos_filter*)this );
  SERIALIZE_ENUM(ft_content_mode::type, mode_);
}

ostream& ftcontent_filter::put( ostream &o ) const {
  BEGIN_PUT( o, ftcontent_filter );
  PUT_ATTR( o, mode ) << endl;
  END_PUT( o );
}

ftdiacritics_option::ftdiacritics_option(
  QueryLoc const &loc,
  ft_diacritics_mode::type mode
) :
  ftmatch_option( loc ),
  mode_( mode )
{
}

ft_visit_result::type ftdiacritics_option::accept( ftnode_visitor &v ) {
  BEGIN_VISIT( v );
  END_VISIT( v );
}

ftnode_t ftdiacritics_option::clone(user_function* udf,  expr::substitution_t& ) const {
  return new ftdiacritics_option( get_loc(), mode_ );
}

ostream& ftdiacritics_option::put( ostream &o ) const {
  BEGIN_PUT( o, ftdiacritics_option );
  PUT_ATTR( o, mode ) << endl;
  END_PUT( o );
}

void ftdiacritics_option::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftmatch_option*)this );
  SERIALIZE_ENUM(ft_diacritics_mode::type,mode_);
}

ftdistance_filter::ftdistance_filter(
  QueryLoc const &loc,
  ftrange *range,
  ft_unit::type unit
) :
  ftpos_filter( loc ),
  range_( range ),
  unit_( unit )
{
  ZORBA_ASSERT( range_ );
}

ftdistance_filter::~ftdistance_filter() {
  delete range_;
}

ft_visit_result::type ftdistance_filter::accept( ftnode_visitor &v ) {
  BEGIN_VISIT( v );
  ACCEPT( range_, v );
  END_VISIT( v );
}

ftnode_t ftdistance_filter::clone(user_function* udf,  expr::substitution_t &s ) const {
  return new ftdistance_filter( get_loc(), clone_ptr_if( range_, udf, s ), unit_ );
}

ostream& ftdistance_filter::put( ostream &o ) const {
  BEGIN_PUT( o, ftcontent_filter );
  PUT_ATTR( o, unit );
  INDENT_PUT( o );
  PUT_NODE( o, range_ );
  OUTDENT_END_PUT( o );
}

void ftdistance_filter::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftpos_filter*)this );
  ar & range_;
  SERIALIZE_ENUM(ft_unit::type,unit_);
}

ftextension_selection::ftextension_selection(
  QueryLoc const &loc,
  rchandle<PragmaList> const &pragmas,
  ftselection *selection
) :
  ftprimary( loc ),
  pragmas_( pragmas ),
  ftselection_( selection )
{
}

ftextension_selection::~ftextension_selection() {
  delete ftselection_;
}

ft_visit_result::type ftextension_selection::accept( ftnode_visitor &v ) {
  BEGIN_VISIT( v );
  ACCEPT( ftselection_, v );
  END_VISIT( v );
}

ftnode_t ftextension_selection::clone(user_function* udf,  expr::substitution_t &s ) const {
  return new ftextension_selection(
             get_loc(), pragmas_, clone_ptr_if( ftselection_, udf, s )
  );
}

ostream& ftextension_selection::put( ostream &o ) const {
  BEGIN_INDENT_PUT( o, ftextension_selection );
  PUT_NODE( o, ftselection_ );
  OUTDENT_END_PUT( o );
}

void ftextension_selection::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftprimary*)this );
  //ar & pragmas_;
  ar & ftselection_;
}

ftextension_option::ftextension_option(
  QueryLoc const &loc,
  rchandle<QName> const &qname,
  zstring const &val
) :
  ftmatch_option( loc ),
  qname_( qname ),
  val_( val )
{
}

ft_visit_result::type ftextension_option::accept( ftnode_visitor &v ) {
  BEGIN_VISIT( v );
  END_VISIT( v );
}

ftnode_t ftextension_option::clone(user_function* udf,  expr::substitution_t& ) const {
  return new ftextension_option( get_loc(), qname_, val_ );
}

ostream& ftextension_option::put( ostream &o ) const {
  BEGIN_PUT( o, ftextension_option )
  //<< " qname=" << qname_
    << " val=" << val_ << endl;
  END_PUT( o );
}

void ftextension_option::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftmatch_option*)this );
  //ar & qname_;
  ar & val_;
}

ftlanguage_option::ftlanguage_option(
  QueryLoc const &loc,
  zstring const &lang
) :
  ftmatch_option( loc )
{
  if ( !GenericCast::instance()->castableToLanguage( lang ) )
    throw XQUERY_EXCEPTION(
      err::XPTY0004,
      ERROR_PARAMS(
        ZED( BadType_23o ), lang, ZED( NoCastTo_45o ), "xs:language"
      ),
      ERROR_LOC( loc )
    );
  if ( !(lang_ = locale::find_lang( lang )) )
    throw XQUERY_EXCEPTION(
      err::FTST0009, ERROR_PARAMS( lang ), ERROR_LOC( loc )
    );
}

ftlanguage_option::ftlanguage_option(
  QueryLoc const &loc,
  iso639_1::type lang
) :
  ftmatch_option( loc ),
  lang_( lang )
{
}

void ftlanguage_option::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftmatch_option*)this );
  SERIALIZE_ENUM(locale::iso639_1::type,lang_);
}


ft_visit_result::type ftlanguage_option::accept( ftnode_visitor &v ) {
  BEGIN_VISIT( v );
  END_VISIT( v );
}

ftnode_t ftlanguage_option::clone(user_function* udf,  expr::substitution_t& ) const {
  return new ftlanguage_option( get_loc(), lang_ );
}

ostream& ftlanguage_option::put( ostream &o ) const {
  BEGIN_PUT( o, ftlanguage_option );
  PUT_ATTR( o, lang ) << endl;
  END_PUT( o );
}

void ftmatch_option::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftnode*)this );
}

ftmatch_options::ftmatch_options( QueryLoc const &loc ) :
  ftnode( loc ),
  current_extension_options_( &extension_options_ )
{
}

ft_visit_result::type ftmatch_options::accept( ftnode_visitor &v ) {
  BEGIN_VISIT( v );
  ACCEPT( case_option_, v );
  ACCEPT( diacritics_option_, v );
  ACCEPT_SEQ( ftextension_option_list_t, *current_extension_options_, v );
  ACCEPT( language_option_, v );
  ACCEPT( stem_option_, v );
  ACCEPT( stop_word_option_, v );
  ACCEPT( thesaurus_option_, v );
  ACCEPT( wild_card_option_, v );
  END_VISIT( v );
}

ftnode_t ftmatch_options::clone(user_function* udf,  expr::substitution_t &s ) const {
  unique_ptr<ftmatch_options> p( new ftmatch_options( get_loc() ) );
  if ( case_option_ )
    p->set_case_option( clone_ptr( case_option_, udf, s ) );
  if ( diacritics_option_ )
    p->set_diacritics_option( clone_ptr( diacritics_option_, udf, s ) );

  FOR_EACH( ftextension_option_list_t, i, extension_options_ )
    p->add_extension_option( clone_ptr( *i, udf, s ) );
  if ( current_extension_options_ != &extension_options_ )
    p->set_extension_options( current_extension_options_ );

  if ( language_option_ )
    p->set_language_option( clone_ptr( language_option_, udf, s ) );
  if ( stem_option_ )
    p->set_stem_option( clone_ptr( stem_option_, udf, s ) );
  if ( stop_word_option_ )
    p->set_stop_word_option( clone_ptr( stop_word_option_, udf, s ) );
  if ( thesaurus_option_ )
    p->set_thesaurus_option( clone_ptr( thesaurus_option_, udf, s ) );
  if ( wild_card_option_ )
    p->set_wild_card_option( clone_ptr( wild_card_option_, udf, s ) );
  return p.release();
}

ostream& ftmatch_options::put( ostream &o ) const {
  BEGIN_INDENT_PUT( o, ftmatch_options );
  PUT_NODE( o, case_option_ );
  PUT_NODE( o, diacritics_option_ );
  PUT_SEQ ( o, ftextension_option_list_t, *current_extension_options_ );
  PUT_NODE( o, language_option_ );
  PUT_NODE( o, stem_option_ );
  PUT_NODE( o, stop_word_option_ );
  PUT_NODE( o, thesaurus_option_ );
  PUT_NODE( o, wild_card_option_ );
  OUTDENT_END_PUT( o );
}

void ftmatch_options::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftnode*)this );
  ar & case_option_;
  ar & diacritics_option_;
  ar & extension_options_;
  if ( !ar.is_serializing_out() )
    current_extension_options_ = &extension_options_;
  ar & language_option_;
  ar & stem_option_;
  ar & stop_word_option_;
  ar & thesaurus_option_;
  ar & wild_card_option_;
}

void ftmatch_options::set_missing_defaults() {
  if ( !case_option_ )
    case_option_ = new ftcase_option( get_loc() );
  if ( !diacritics_option_ )
    diacritics_option_ = new ftdiacritics_option( get_loc() );
  // extension_option_
  // language_option_
  if ( !stem_option_ )
    stem_option_ = new ftstem_option( get_loc() );
  if ( !stop_word_option_ )
    stop_word_option_ = new ftstop_word_option( get_loc() );
  // thesaurus_option_
  if ( !wild_card_option_ )
    wild_card_option_ = new ftwild_card_option( get_loc() );
}

ftmild_not::ftmild_not( QueryLoc const &loc, ftnode_list_t &list ) :
  ftnode_list( loc, list )
{
}

ft_visit_result::type ftmild_not::accept( ftnode_visitor &v ) {
  BEGIN_VISIT( v );
  ACCEPT_SEQ( ftnode_list_t, get_node_list(), v );
  END_VISIT( v );
}

ftnode_t ftmild_not::clone(user_function* udf,  expr::substitution_t &s ) const {
  ftnode_list_t copy;
  clone_list( get_node_list(), &copy, udf, s );
  return new ftmild_not( get_loc(), copy );
}

ostream& ftmild_not::put( ostream &o ) const {
  BEGIN_INDENT_PUT( o, ftmild_not );
  PUT_SEQ( o, ftnode_list_t, get_node_list() );
  OUTDENT_END_PUT( o );
}

void ftmild_not::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftnode_list*)this );
}

ftnode_list::ftnode_list( QueryLoc const &loc, ftnode_list_t &list ) :
  ftnode( loc )
{
  list_.swap( list );
}

ftnode_list::~ftnode_list() {
  ztd::delete_ptr_seq( list_ );
}

void ftnode_list::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftnode*)this );
  ar & list_;
}

ftor::ftor( QueryLoc const &loc, ftnode_list_t &list ) :
  ftnode_list( loc, list )
{
}

ft_visit_result::type ftor::accept( ftnode_visitor &v ) {
  BEGIN_VISIT( v );
  ACCEPT_SEQ( ftnode_list_t, get_node_list(), v );
  END_VISIT( v );
}

ftnode_t ftor::clone(user_function* udf,  expr::substitution_t &s ) const {
  ftnode_list_t copy;
  clone_list( get_node_list(), &copy, udf, s );
  return new ftor( get_loc(), copy );
}

ostream& ftor::put( ostream &o ) const {
  BEGIN_INDENT_PUT( o, ftor );
  PUT_SEQ( o, ftnode_list_t, get_node_list() );
  OUTDENT_END_PUT( o );
}

void ftor::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftnode_list*)this );
}

ftorder_filter::ftorder_filter( QueryLoc const &loc ) :
  ftpos_filter( loc )
{
}

ft_visit_result::type ftorder_filter::accept( ftnode_visitor &v ) {
  BEGIN_VISIT( v );
  END_VISIT( v );
}

ftnode_t ftorder_filter::clone(user_function* udf,  expr::substitution_t &s ) const {
  return new ftorder_filter( get_loc() );
}

ostream& ftorder_filter::put( ostream &o ) const {
  BEGIN_PUT( o, ftorder_filter ) << endl;
  END_PUT( o );
}

void ftorder_filter::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftpos_filter*)this );
}

ftpos_filter::ftpos_filter( QueryLoc const &loc ) :
  ftnode( loc )
{
}

void ftpos_filter::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftnode*)this );
}

ftprimary_with_options::ftprimary_with_options( QueryLoc const &loc ) :
  ftnode( loc ),
  primary_( nullptr ),
  match_options_( new ftmatch_options( loc ) ),
  weight_( nullptr )
{
}

ftprimary_with_options::~ftprimary_with_options() {
  delete primary_;
  delete match_options_;
  delete weight_;
}

ft_visit_result::type ftprimary_with_options::accept( ftnode_visitor &v ) {
  BEGIN_VISIT( v );
  ACCEPT( primary_, v );
  ACCEPT( match_options_, v );
  ACCEPT( weight_, v );
  END_VISIT( v );
}

ftnode_t ftprimary_with_options::clone(user_function* udf,  expr::substitution_t &s ) const {
  unique_ptr<ftprimary_with_options> p(
    new ftprimary_with_options( get_loc() )
  );
  p->set_match_options( clone_ptr( match_options_, udf, s ) );
  p->set_primary( clone_ptr( primary_, udf, s ) );
  if ( weight_ )
    p->set_weight( clone_ptr( weight_, udf, s ) );
  return p.release();
}

ostream& ftprimary_with_options::put( ostream &o ) const {
  BEGIN_INDENT_PUT( o, ftprimary_with_options );
  PUT_NODE( o, primary_ );
  PUT_NODE( o, match_options_ );
  PUT_NODE( o, weight_ );
  OUTDENT_END_PUT( o );
}

ftprimary_with_options::ftprimary_with_options(serialization::Archiver& ar)
  :
  ftnode(ar),
  primary_(NULL),
  match_options_(NULL),
  weight_(NULL)
{
}

void ftprimary_with_options::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftnode*)this );
  ar & primary_;
  ar & match_options_;
  ar & weight_;
}

ftrange::ftrange(
  QueryLoc const &loc,
  ft_range_mode::type mode,
  expr* const &expr1,
  expr* expr2
) :
  ftnode( loc ),
  mode_( mode ),
  expr1_( expr1 ),
  expr2_( expr2 )
{
  ZORBA_ASSERT( expr1_ );
}

ft_visit_result::type ftrange::accept( ftnode_visitor &v ) {
  BEGIN_VISIT( v );
  EV_ACCEPT( expr1_, v );
  EV_ACCEPT( expr2_, v );
  END_VISIT( v );
}

ftnode_t ftrange::clone(user_function* udf,  expr::substitution_t &s ) const {
  return new ftrange(
    get_loc(), mode_,
    clone_ptr( expr1_, udf, s ),
    clone_ptr_if( expr2_, udf, s )
  );
  // TODO: do PlanIter_t's have to be cloned?
}

ostream& ftrange::put( ostream &o ) const {
  BEGIN_INDENT_PUT( o, ftrange );
  PUT_EXPR( o, expr1_ );
  PUT_EXPR( o, expr2_ );
  OUTDENT_END_PUT( o );
}

void ftrange::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftnode*)this );
  SERIALIZE_ENUM(ft_range_mode::type,mode_);
  //ar & expr1_;
  //ar & expr2_;
  ar & iter1_;
  ar & iter2_;
}

ftscope_filter::ftscope_filter(
  QueryLoc const &loc, ft_scope::type scope, ft_big_unit::type unit
) :
  ftpos_filter( loc ),
  scope_( scope ),
  unit_( unit )
{
}

ft_visit_result::type ftscope_filter::accept( ftnode_visitor &v ) {
  BEGIN_VISIT( v );
  END_VISIT( v );
}

ftnode_t ftscope_filter::clone(user_function* udf,  expr::substitution_t &s ) const {
  return new ftscope_filter( get_loc(), scope_, unit_ );
}

ostream& ftscope_filter::put( ostream &o ) const {
  BEGIN_PUT( o, ftscope_filter );
  PUT_ATTR( o, scope );
  PUT_ATTR( o, unit ) << endl;
  END_PUT( o );
}

void ftscope_filter::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftpos_filter*)this );
  SERIALIZE_ENUM(ft_scope::type,scope_);
  SERIALIZE_ENUM(ft_big_unit::type,unit_);
}

ftselection::ftselection(
  QueryLoc const &loc,
  ftnode *ftor,
  ftpos_filter_list_t const &list
) :
  ftprimary( loc ),
  ftor_( ftor )
{
  //
  // From the spec, section 3.6:
  //
  //  In a group of multiple adjacent positional filters, FTOrder filters are
  //  applied first, and then the other positional filters are applied from
  //  left to right, skipping the FTOrder filters.
  //
  // Hence we push the FTOrder filter(s) first followed by the non-FTOrder
  // filter(s).
  //
  FOR_EACH( ftpos_filter_list_t, i, list ) {
    if ( dynamic_cast<ftorder_filter*>( *i ) )
      list_.push_back( *i );
  }
  FOR_EACH( ftpos_filter_list_t, i, list ) {
    if ( !dynamic_cast<ftorder_filter*>( *i ) )
      list_.push_back( *i );
  }
}

ftselection::~ftselection() {
  delete ftor_;
  ztd::delete_ptr_seq( list_ );
}

ft_visit_result::type ftselection::accept( ftnode_visitor &v ) {
  BEGIN_VISIT( v );
  ACCEPT( ftor_, v );
  ACCEPT_SEQ( ftpos_filter_list_t, list_, v );
  END_VISIT( v );
}

ftnode_t ftselection::clone(user_function* udf,  expr::substitution_t &s ) const {
  return new ftselection( get_loc(), ftor_->clone( udf, s ).release(), list_ );
}

ostream& ftselection::put( ostream &o ) const {
  BEGIN_INDENT_PUT( o, ftselection );
  PUT_NODE( o, ftor_ );
  PUT_SEQ( o, ftpos_filter_list_t, list_ );
  OUTDENT_END_PUT( o );
}

void ftselection::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftprimary*)this );
  ar & ftor_;
  ar & list_;
}

ftstem_option::ftstem_option(
  QueryLoc const &loc,
  ft_stem_mode::type mode
) :
  ftmatch_option( loc ),
  mode_( mode )
{
}

ft_visit_result::type ftstem_option::accept( ftnode_visitor &v ) {
  BEGIN_VISIT( v );
  END_VISIT( v );
}

ftnode_t ftstem_option::clone(user_function* udf,  expr::substitution_t& ) const {
  return new ftstem_option( get_loc(), mode_ );
}

ostream& ftstem_option::put( ostream &o ) const {
  BEGIN_PUT( o, ftstem_option );
  PUT_ATTR( o, mode ) << endl;
  END_PUT( o );
}

void ftstem_option::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftmatch_option*)this );
  SERIALIZE_ENUM(ft_stem_mode::type,mode_);
}

ftstop_words::ftstop_words(
  QueryLoc const &loc,
  zstring const &uri,
  list_t const &stop_word_list,
  ft_stop_words_unex::type mode
) :
  ftnode( loc ),
  uri_( uri ),
  list_( stop_word_list ),
  mode_( mode )
{
  ZORBA_ASSERT( !uri_.empty() || !list_.empty() );
}

ft_visit_result::type ftstop_words::accept( ftnode_visitor &v ) {
  BEGIN_VISIT( v );
  END_VISIT( v );
}

ftnode_t ftstop_words::clone(user_function* udf,  expr::substitution_t& ) const {
  return new ftstop_words( get_loc(), uri_, list_, mode_ );
}

ostream& ftstop_words::put( ostream &o ) const {
  BEGIN_PUT( o, ftstop_words );
  PUT_ATTR( o, uri );
  PUT_ATTR( o, mode );
  PUT_LABEL( o, stop_words ) << flush;
  FOR_EACH( list_t, i, list_ )
    o << *i << ' ';
  o << endl;
  END_PUT( o );
}

void ftstop_words::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftnode*)this );
  ar & uri_;
  ar & list_;
  SERIALIZE_ENUM(ft_stop_words_unex::type,mode_);
}

ftstop_word_option::ftstop_word_option(
  QueryLoc const &loc,
  ft_stop_words_mode::type mode
) :
  ftmatch_option( loc ),
  mode_( mode )
{
}

ftstop_word_option::ftstop_word_option(
  QueryLoc const &loc,
  list_t &stop_word_list,
  ft_stop_words_mode::type mode
) :
  ftmatch_option( loc ),
  mode_( mode )
{
  stop_words_.swap( stop_word_list );
}

ftstop_word_option::~ftstop_word_option() {
  ztd::delete_ptr_seq( stop_words_ );
}

ft_visit_result::type ftstop_word_option::accept( ftnode_visitor &v ) {
  BEGIN_VISIT( v );
  ACCEPT_SEQ( list_t, stop_words_, v );
  END_VISIT( v );
}

ftnode_t ftstop_word_option::clone(user_function* udf,  expr::substitution_t &s ) const {
  list_t copy;
  clone_list( stop_words_, &copy, udf, s );
  return new ftstop_word_option( get_loc(), copy, mode_ );
}

ostream& ftstop_word_option::put( ostream &o ) const {
  BEGIN_PUT( o, ftstop_word_option );
  PUT_ATTR( o, mode );
  if ( stop_words_.empty() )
    o << endl;
  else {
    INDENT_PUT( o );
    PUT_SEQ( o, list_t, stop_words_ );
    OUTDENT_PUT( o );
  }
  END_PUT( o );
}

void ftstop_word_option::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftmatch_option*)this );
  ar & stop_words_;
  SERIALIZE_ENUM(ft_stop_words_mode::type,mode_);
}

ftthesaurus_id::ftthesaurus_id(
  QueryLoc const &loc,
  zstring const &uri,
  zstring const &relationship,
  ftrange *levels
) :
  ftnode( loc ),
  uri_( uri ),
  levels_( levels )
{
  ascii::to_lower( relationship, &relationship_ );
}

ftthesaurus_id::~ftthesaurus_id() {
  delete levels_;
}

ft_visit_result::type ftthesaurus_id::accept( ftnode_visitor &v ) {
  BEGIN_VISIT( v );
  ACCEPT( levels_, v );
  END_VISIT( v );
}

ftnode_t ftthesaurus_id::clone(user_function* udf,  expr::substitution_t &s ) const {
  return new ftthesaurus_id(
         get_loc(), uri_, relationship_, clone_ptr_if( levels_, udf, s )
  );
}

ostream& ftthesaurus_id::put( ostream &o ) const {
  BEGIN_PUT( o, ftthesaurus_id );
  PUT_ATTR( o, uri );
  PUT_ATTR( o, relationship );
  INDENT_PUT( o );
  PUT_NODE( o, levels_ );
  OUTDENT_END_PUT( o );
}

void ftthesaurus_id::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftnode*)this );
  ar & uri_;
  ar & relationship_;
  ar & levels_;
}

ftthesaurus_option::ftthesaurus_option(
  QueryLoc const &loc,
  ftthesaurus_id *default_tid,
  thesaurus_id_list_t &list,
  bool no_thesaurus_arg
) :
  ftmatch_option( loc ),
  default_tid_( default_tid ),
  no_thesaurus_( no_thesaurus_arg )
{
  thesaurus_id_list_.swap( list );
}

ftthesaurus_option::~ftthesaurus_option() {
  delete default_tid_;
  ztd::delete_ptr_seq( thesaurus_id_list_ );
}

ft_visit_result::type ftthesaurus_option::accept( ftnode_visitor &v ) {
  BEGIN_VISIT( v );
  ACCEPT( default_tid_, v );
  ACCEPT_SEQ( thesaurus_id_list_t, thesaurus_id_list_, v );
  END_VISIT( v );
}

ftnode_t ftthesaurus_option::clone(user_function* udf,  expr::substitution_t &s ) const {
  thesaurus_id_list_t copy;
  clone_list( thesaurus_id_list_, &copy, udf, s );
  return new ftthesaurus_option(
         get_loc(), clone_ptr_if( default_tid_, udf, s ), copy, no_thesaurus_
  );
}

ostream& ftthesaurus_option::put( ostream &o ) const {
  BEGIN_PUT( o, ftthesaurus_option );
  PUT_BOOL( o, no_thesaurus );
  INDENT_PUT( o );
  PUT_NODE( o, default_tid_ );
  PUT_SEQ( o, thesaurus_id_list_t, thesaurus_id_list_ );
  OUTDENT_END_PUT( o );
}

void ftthesaurus_option::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftmatch_option*)this );
  ar & default_tid_;
  ar & thesaurus_id_list_;
  ar & no_thesaurus_;
}

ftunary_not::ftunary_not( QueryLoc const &loc, ftnode *n ) :
  ftnode( loc ), subnode_( n )
{
}

ftunary_not::~ftunary_not() {
  delete subnode_;
}

ft_visit_result::type ftunary_not::accept( ftnode_visitor &v ) {
  BEGIN_VISIT( v );
  ACCEPT( subnode_, v );
  END_VISIT( v );
}

ftnode_t ftunary_not::clone(user_function* udf,  expr::substitution_t &s ) const {
  return new ftunary_not( get_loc(), subnode_->clone( udf, s ).release() );
}

ostream& ftunary_not::put( ostream &o ) const {
  BEGIN_INDENT_PUT( o, ftunary_not );
  PUT_NODE( o, subnode_ );
  OUTDENT_END_PUT( o );
}

void ftunary_not::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftnode*)this );
  ar & subnode_;
}

ftweight::ftweight( QueryLoc const &loc, expr* const &weight_expr ) :
  ftnode( loc ), weight_expr_( weight_expr )
{
}

ft_visit_result::type ftweight::accept( ftnode_visitor &v ) {
  BEGIN_VISIT( v );
  EV_ACCEPT( weight_expr_, v );
  END_VISIT( v );
}

ftnode_t ftweight::clone(user_function* udf,  expr::substitution_t &s ) const {
  return new ftweight( get_loc(), weight_expr_->clone( udf, s ) );
}

ostream& ftweight::put( ostream &o ) const {
  BEGIN_INDENT_PUT( o, ftweight );
  PUT_EXPR( o, weight_expr_ );
  OUTDENT_END_PUT( o );
}

void ftweight::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftnode*)this );
  //ar & weight_expr_;
  ar & weight_iter_;
}

ftwild_card_option::ftwild_card_option(
  QueryLoc const &loc,
  ft_wild_card_mode::type mode
) :
  ftmatch_option( loc ),
  mode_( mode )
{
}

ft_visit_result::type ftwild_card_option::accept( ftnode_visitor &v ) {
  BEGIN_VISIT( v );
  END_VISIT( v );
}

ftnode_t ftwild_card_option::clone(user_function* udf,  expr::substitution_t &s ) const {
  return new ftwild_card_option( get_loc(), mode_ );
}

ostream& ftwild_card_option::put( ostream &o ) const {
  BEGIN_PUT( o, ftwild_card_option );
  PUT_ATTR( o, mode ) << endl;
  END_PUT( o );
}

void ftwild_card_option::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftmatch_option*)this );
  SERIALIZE_ENUM(ft_wild_card_mode::type,mode_);
}

ftwindow_filter::ftwindow_filter(
  QueryLoc const &loc,
  expr* const &window_expr,
  ft_unit::type unit )
:
  ftpos_filter( loc ),
  window_expr_( window_expr ),
  unit_( unit )
{
  ZORBA_ASSERT( window_expr );
}

ft_visit_result::type ftwindow_filter::accept( ftnode_visitor &v ) {
  BEGIN_VISIT( v );
  EV_ACCEPT( window_expr_, v );
  END_VISIT( v );
}

ftnode_t ftwindow_filter::clone(user_function* udf,  expr::substitution_t &s ) const {
  return new ftwindow_filter(get_loc(), window_expr_->clone( udf, s ), unit_ );
}

ostream& ftwindow_filter::put( ostream &o ) const {
  BEGIN_PUT( o, ftwindow_filter );
  PUT_ATTR( o, unit );
  INDENT_PUT( o );
  PUT_EXPR( o, window_expr_ );
  OUTDENT_END_PUT( o );
}

void ftwindow_filter::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftpos_filter*)this );
  //ar & window_expr_;
  SERIALIZE_ENUM(ft_unit::type,unit_);
  ar & window_iter_;
}

ftwords::ftwords(
  QueryLoc const &loc,
  expr* const &value_expr,
  ft_anyall_mode::type mode
) :
  ftnode( loc ),
  value_expr_( value_expr ),
  mode_( mode )
{
}

ft_visit_result::type ftwords::accept( ftnode_visitor &v ) {
  BEGIN_VISIT( v );
  EV_ACCEPT( value_expr_, v );
  END_VISIT( v );
}

ftnode_t ftwords::clone(user_function* udf,  expr::substitution_t &s ) const {
  return new ftwords( get_loc(), value_expr_->clone( udf, s ), mode_ );
}

ostream& ftwords::put( ostream &o ) const {
  BEGIN_PUT( o, ftwords );
  PUT_ATTR( o, mode );
  INDENT_PUT( o );
  PUT_EXPR( o, value_expr_ );
  OUTDENT_END_PUT( o );
}

void ftwords::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftnode*)this );
  SERIALIZE_ENUM( ft_anyall_mode::type, mode_ );
  //ar & value_expr_;
  ar & value_iter_;
}

ftwords_times::ftwords_times(
  QueryLoc const &loc,
  ftwords *ftwords,
  ftrange *fttimes
) :
  ftprimary( loc ),
  ftwords_( ftwords ),
  fttimes_( fttimes )
{
  ZORBA_ASSERT( ftwords_ );
}

ftwords_times::~ftwords_times() {
  delete ftwords_;
  delete fttimes_;
}

ft_visit_result::type ftwords_times::accept( ftnode_visitor &v ) {
  BEGIN_VISIT( v );
  ACCEPT( ftwords_, v );
  ACCEPT( fttimes_, v );
  END_VISIT( v );
}

ftnode_t ftwords_times::clone(user_function* udf,  expr::substitution_t &s ) const {
  return new ftwords_times(get_loc(),
                           clone_ptr( ftwords_, udf, s ),
                           clone_ptr_if( fttimes_, udf, s )
  );
}

ostream& ftwords_times::put( ostream &o ) const {
  BEGIN_INDENT_PUT( o, ftwords_times );
  PUT_NODE( o, ftwords_ );
  PUT_NODE( o, fttimes_ );
  OUTDENT_END_PUT( o );
}

void ftwords_times::serialize( serialization::Archiver &ar ) {
  serialize_baseclass( ar, (ftprimary*)this );
  ar & ftwords_;
  ar & fttimes_;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace zorba
/* vim:set et sw=2 ts=2: */
