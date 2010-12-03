import module namespace zorba-csv = "http://www.zorba-xquery.com/modules/csv";
import module namespace file="http://www.zorba-xquery.com/modules/file";

zorba-csv:parse(file:read-text(fn:resolve-uri("Hospital Outpatient Payments for 61 Commonly Performed Procedures, CY 2008 Data.csv")), 
<options>
  <first-row-is-header line="5-8"/>
  <start-from-row line="9"/>
  <csv separator=","
       quote-char="&quot;"
       quote-escape="&quot;&quot;"/>
</options>
)

