#ifndef ZORBA_ERROR_PRINTER_H
#define ZORBA_ERROR_PRINTER_H

#include <ostream>

namespace zorba {

  class QueryException;

  class ErrorPrinter {
      public:
          static std::ostream&
          print(const QueryException& lException,
                std::ostream&         lOut,
                bool                  aIndent,
                bool                  aAsXml);

  }; /* class ErrorPrinter */
} /* namespace zorba */

#endif
