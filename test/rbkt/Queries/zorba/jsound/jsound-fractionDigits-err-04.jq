import module namespace jsd = "http://jsound.io/modules/jsound"; 

let $jsd :=
  {
    "$namespace" : "http://www.example.com/my-schema",
    "$types" : [
      {
        "$kind" : "atomic",
        "$name" : "foo",
        "$baseType" : "decimal",
        "$fractionDigits" : 2
      },
      {
        "$kind" : "atomic",
        "$name" : "bar",
        "$baseType" : "foo",
        "$fractionDigits" : 3 (: must be <= 2 :)
      }
    ]
  }

let $instance := 12345

return jsd:validate( $jsd, "foo", $instance )

(: vim:set syntax=xquery et sw=2 ts=2: :)
