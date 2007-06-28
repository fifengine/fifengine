{
  if ($2 == "table") {
    last_table = $3
  }
  else {
    if ($0 ~ /  /) {
      print last_table " " $2
    }
    else {
      print $2
    }
  }
}
