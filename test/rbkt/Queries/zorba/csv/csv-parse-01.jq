import module namespace csv = "http://zorba.io/modules/csv";
import module namespace file = "http://expath.org/ns/file";

declare variable $rbktPath as xs:string external;

let $file := concat( $rbktPath, "/Queries/zorba/csv/sample_files/csv-01.txt" )
let $values := file:read-text( $file )
return csv:parse( $values )

(: vim:set syntax=xquery et sw=2 ts=2: :)
