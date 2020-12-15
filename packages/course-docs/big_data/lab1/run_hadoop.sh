DIR=./output
if [ -d "$DIR" ]; then
    printf '%s\n' "Removing output directory"
    rm -rf "$DIR"
fi

if [ $1 == 1 ]; then
	hadoop jar /usr/local/Cellar/hadoop/2.7.2/libexec/share/hadoop/tools/lib/hadoop-streaming-2.7.2.jar -files part1/mapper.py,part1/reducer.py -mapper part1/mapper.py -reducer part1/reducer.py -input data/book.txt -output output
fi

if [ $1 == 2 ]; then
	hadoop jar /usr/local/Cellar/hadoop/2.7.2/libexec/share/hadoop/tools/lib/hadoop-streaming-2.7.2.jar -files part2/mapper.py,part2/reducer.py -mapper part2/mapper.py -reducer part2/reducer.py -input data/book.txt -output output
fi
