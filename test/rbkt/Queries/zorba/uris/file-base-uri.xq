import module namespace file="http://expath.org/ns/file";
import module namespace x="http://www.zorba-xquery.com/modules/xml";
import schema namespace opt="http://www.zorba-xquery.com/modules/xml-options";

let $uri := fn:resolve-uri( "input.xml", fn:static-base-uri() )
let $content := file:read-text($uri)
let $doc := x:parse($content,
     <opt:options><opt:base-uri opt:value="{$uri}"/></opt:options>)
return (fn:base-uri($doc) eq $uri)
