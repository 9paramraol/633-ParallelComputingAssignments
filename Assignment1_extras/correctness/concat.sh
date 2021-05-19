echo  "" > "./final"
for entry in "./all_ranks"/*
do
  cat "$entry" >> "./final"
  rm -f "$entry"
done

