import module namespace file = "http://www.zorba-xquery.com/modules/file";

declare variable $xqdocXmlPath as xs:string := fn:concat("", file:directory-separator(), "xml");

$xqdocXmlPath = "/xml" or $xqdocXmlPath = "\xml"
