(: flwor has if statement in return collapsed into existing where as 
joining of original where and if condition through boolean AND operation :)

for $x in (1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
where $x < 8
return if($x > 2) then $x else ()
