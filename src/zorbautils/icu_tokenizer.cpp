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

#include <cctype>
#include <memory>                       /* for auto_ptr */
#include <unicode/unistr.h>

#define DEBUG_TOKENIZER 0
#if DEBUG_TOKENIZER
#include <iostream>
#endif

#include <zorba/error.h>

#include "util/stl_util.h"
#include "zorbaerrors/error_manager.h"
#include "zorbautils/icu_tokenizer.h"
#include "zorbautils/mutex.h"
#include "zorbautils/unicode_util.h"

using namespace std;
U_NAMESPACE_USE
using namespace zorba::locale;

namespace zorba {

///////////////////////////////////////////////////////////////////////////////

namespace {

class token {
public:
  void append( char const *s, int slen ) {
    value_.append( s, slen );
  }

  void clear() {
    value_.clear();
  }

  bool empty() {
    return value_.empty();
  }

  void send_to( Tokenizer::Callback &callback ) {
    if ( !empty() ) {
#     if DEBUG_TOKENIZER
      cout << "TOKEN: \"" << value_ << "\"\n";
#     endif
      callback( value_.c_str(), value_.length(), pos_, sent_, para_ );
      clear();
    }
  }

  void set( char const *s, int slen, int pos, int sent, int para ) {
    clear();
    append( s, slen );
    pos_  = pos;
    sent_ = sent;
    para_ = para;
  }

private:
  string value_;
  int pos_, sent_, para_;
};

} // anonymous namespace

///////////////////////////////////////////////////////////////////////////////

/**
 * For a given language, get its corresponding ICU Locale.
 *
 * @param lang The language to get the ICU locale for.
 * @return Returns said Locale.
 */
static Locale const& get_icu_locale_for( iso639_1::type lang ) {
  typedef map<iso639_1::type,Locale> locale_cache_t;
  static locale_cache_t locale_cache;
  static Mutex mutex;

  if ( lang == iso639_1::unknown )
    lang = get_host_lang();

  AutoMutex const lock( &mutex );

  locale_cache_t::const_iterator const i = locale_cache.find( lang );
  if ( i != locale_cache.end() )
    return i->second;

  iso3166_1::type const country_code = get_host_country();
  char const *const country = country_code != iso3166_1::unknown ?
    iso3166_1::string_of[ country_code ] : NULL;
  Locale &icu_locale = locale_cache[ lang ];
  icu_locale = Locale( iso639_1::string_of[ lang ], country );
  return icu_locale;
}

/**
 * Creates the ICU word & sentence RuleBasedBreakIterators for the given
 * language.
 *
 * @param lang The language to create the RuleBasedBreakIterators for.
 * @param result The created iterators are put here.
 */
void ICU_Tokenizer::create_iterators( iso639_1::type lang,
                                      ICU_Iterators &result ) {
  typedef auto_ptr<RuleBasedBreakIterator> rbbi_ptr;

  Locale const &icu_locale = get_icu_locale_for( lang );
  UErrorCode status = U_ZERO_ERROR;

  rbbi_ptr word_it(
    dynamic_cast<RuleBasedBreakIterator*>(
      BreakIterator::createWordInstance( icu_locale, status )
    )
  );
  if ( U_FAILURE( status ) )
    ZORBA_ERROR( XQP0036_BREAKITERATOR_CREATION_FAILED );

  rbbi_ptr sent_it(
    dynamic_cast<RuleBasedBreakIterator*>(
      BreakIterator::createSentenceInstance( Locale::getUS(), status )
    )
  );
  if ( U_FAILURE( status ) )
    ZORBA_ERROR( XQP0036_BREAKITERATOR_CREATION_FAILED );

  result.word_ = word_it.release();
  result.sent_ = sent_it.release();
}

/**
 * Gets the ICU word & sentence RuleBasedBreakIterators for the given language
 * (creating them if necessary).
 *
 * @param lang The language to get the RuleBasedBreakIterators for.
 * @return Returns said iterators.
 */
ICU_Tokenizer::ICU_Iterators&
ICU_Tokenizer::get_iterators_for( iso639_1::type lang ) {
  lang_iters_t::iterator i = lang_iters_.find( lang );
  if ( i != lang_iters_.end() )
    return i->second;
  ICU_Iterators &iters = lang_iters_[ lang ];
  create_iterators( lang, iters );
  return iters;
}

///////////////////////////////////////////////////////////////////////////////

ICU_Tokenizer::ICU_Tokenizer( bool wildcards ) : wildcards_( wildcards ) {
  // do nothing else
}

///////////////////////////////////////////////////////////////////////////////

#define HANDLE_BACKSLASH()            \
  if ( !got_backslash ) ; else {      \
    got_backslash = in_wild = false;  \
    break;                            \
  }

#define IF_GOT_BACKSLASH_APPEND_AND_GOTO(GOTO)  \
  if ( !got_backslash ) ; else {                \
    t.append( utf8_buf.get(), utf8_len );       \
    got_backslash = false;                      \
    goto GOTO;                                  \
  }

#define IS_WORD_BREAK(TYPE,STATUS) \
  ( (STATUS) >= UBRK_WORD_##TYPE && (STATUS) < UBRK_WORD_##TYPE##_LIMIT )

void ICU_Tokenizer::tokenize( char const *utf8_s, int utf8_len,
                              iso639_1::type lang, Callback &callback ) {
  ICU_Iterators &iters = get_iterators_for( lang );

  int32_t utf16_len;
  auto_vec<UChar> const utf16_buf(
    utf8_to_utf16( utf8_s, utf8_len, &utf16_len )
  );

  //
  // ICU bizarrely treats newline and carriage-return as sentence terminators
  // so convert all non-space whitespace characters to space characters.
  //
  UChar *c = utf16_buf.get();
  for ( int i = 0; i < utf16_len; ++i, ++c ) {
    if ( u_isspace( *c ) )
      *c = L' ';
  }

  // This UnicodeString wraps the existing buffer: no copy is made.
  UnicodeString const utf16_s( false, utf16_buf.get(), utf16_len );

  iters.word_->setText( utf16_s );
  int32_t word_start = iters.word_->first(), word_end = iters.word_->next();

  iters.sent_->setText( utf16_s );
  int32_t sent_start = iters.sent_->first(), sent_end = iters.sent_->next();

  token t;

  // True only if the previous token was a backslash ('\').
  bool got_backslash = false;

  // True only if we're actually processing wildcard syntax (started by '.').
  bool in_wild = false;

  // True only if we're within braces (started by '{', ended by '}').
  bool in_brace = false;

  while ( word_end != BreakIterator::DONE ) {
    int32_t const utf16_len = word_end - word_start;
    int32_t utf8_len;
    auto_vec<char> const utf8_buf(
      utf16_to_utf8( utf16_buf.get() + word_start, utf16_len, &utf8_len )
    );
    int32_t const rule_status = iters.word_->getRuleStatus();

    //
    // "Junk" tokens are whitespace and punctuation -- except some punctuation
    // isn't junk when we're tokenizing with XQuery wildcard syntax.
    //
    bool is_junk = false;

#   if DEBUG_TOKENIZER
    cout << "GOT: \"" << string( utf8_buf.get(), utf8_len ) << "\" ";
#   endif

    if ( IS_WORD_BREAK( NONE, rule_status ) ) {
      //
      // "NONE" tokens are what ICU calls whitespace and punctuation.
      //
#     if DEBUG_TOKENIZER
      cout << "(NONE)" << endl;
#     endif
      if ( wildcards_ ) {
        switch ( *utf8_buf ) {
          case '.':
            HANDLE_BACKSLASH();
            in_wild = true;
            goto set_token;
          case '\\':
            HANDLE_BACKSLASH();
            got_backslash = true;
            break;
          case '*':
          case '+':
          case '?':
          case '{':
            if ( in_brace )
              ZORBA_ERROR( FTDY0020 );
            HANDLE_BACKSLASH();
            if ( in_wild ) {
              in_brace = *utf8_buf == '{';
              goto set_token;
            }
            break;
          case '}':
            HANDLE_BACKSLASH();
            if ( in_brace ) {
              in_brace = false;
              goto set_token;
            }
            if ( in_wild )
              ZORBA_ERROR( FTDY0020 );
            break;
          default:
            in_wild = false;
        }
      }
      is_junk = true;
    }

    else if ( IS_WORD_BREAK( NUMBER, rule_status ) ) {
      //
      // "NUMBER" tokens are obviously for numbers.  Note that a sequence of
      // digits containing a ',' (e.g., "1,2") is considered a single token by
      // ICU.
      //
#     if DEBUG_TOKENIZER
      cout << "(NUMBER)" << endl;
#     endif
      IF_GOT_BACKSLASH_APPEND_AND_GOTO( next );
      if ( in_brace ) {
        //
        // Validate that the token matches the regex "[0-9]+,[0-9]+".
        //
        char const *c = utf8_buf.get();
        int i;
        for ( i = 0; i < utf8_len; ++i, ++c ) {
          if ( i && *c == ',' )
            break;
          if ( !isdigit( *c ) )
            ZORBA_ERROR( FTDY0020 );
        }
        if ( i == utf8_len || *c != ',' )
          ZORBA_ERROR( FTDY0020 );
        for ( ++i, ++c; i < utf8_len; ++i, ++c ) {
          if ( !isdigit( *c ) )
            ZORBA_ERROR( FTDY0020 );
        }
      }
    }

    else {
      //
      // "OTHER" tokens are for non-whitespace, non-digits, and non-numbers,
      // i.e., word tokens.
      //
#     if DEBUG_TOKENIZER
      cout << "(OTHER)" << endl;
#     endif
      if ( in_brace )
        ZORBA_ERROR( FTDY0020 );
      IF_GOT_BACKSLASH_APPEND_AND_GOTO( next );
    }

    if ( !in_wild && !got_backslash )
      t.send_to( callback );

set_token:
    if ( !is_junk ) {
      if ( in_wild || got_backslash )
        t.append( utf8_buf.get(), utf8_len );
      else
        t.set( utf8_buf.get(), utf8_len, token_no_++, sent_no_, para_no_ );
    }

next:
    word_start = word_end, word_end = iters.word_->next();
    if ( word_end >= sent_end && sent_end != BreakIterator::DONE ) {
      sent_start = sent_end, sent_end = iters.sent_->next();
      ++sent_no_;
    }
  } // while

  if ( in_brace )
    ZORBA_ERROR( FTDY0020 );
  t.send_to( callback );
}

} // namespace zorba
/* vim:set et sw=2 ts=2: */
