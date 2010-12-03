import module namespace zorba-csv = "http://www.zorba-xquery.com/modules/csv";
import module namespace file="http://www.zorba-xquery.com/modules/file";

zorba-csv:parse(
        file:read-text(fn:resolve-uri("budauth.csv")),
        <options>
  <first-row-is-header/>
  <csv separator=","
       quote-char="&quot;"
       quote-escape="&quot;&quot;"/>
  <xml-nodes>
    <test7:row xmlns:test7="http://www.zorba-xquery/modules/csv">
    </test7:row>
  </xml-nodes>
</options>
)