import org.apache.hadoop.fs.Path;
import org.apache.hadoop.conf.*;
import org.apache.hadoop.io.*;
import org.apache.hadoop.mapreduce.*;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;

import java.util.*;
import java.io.IOException;
import java.io.DataInput;
import java.io.DataOutput;

// Do not change the signature of this class
public class TextAnalyzer extends Configured implements Tool {


    // Replace "?" with your own output key / value types
    // The four template data types are:
    //     <Input Key Type, Input Value Type, Output Key Type, Output Value Type>
    public static class TextMapper extends Mapper<LongWritable, Text, Text, ContextMap> {

        public void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException
        {
            String[] words = value.toString().toLowerCase().replaceAll("[^a-z0-9]", " ").split("[ \t]+");
            ArrayList<String> count= new ArrayList<String>();

            for(int i = 0; i < words.length; i++){
                if(words[i].length() > 0 && !count.contains(words[i])) {
            
                    count.add(words[i]);             
                    ContextMap keyWords = new ContextMap(); 
                    for(int j = 0; j < words.length; j++){ 
                        if(i != j && words[j].length() > 0){ 

                            //add the context word
                            if(keyWords.map.containsKey(new Text(words[j]))) {
                                keyWords.map.put(new Text(words[j]), new IntWritable(((IntWritable)keyWords.map.get(new Text(words[j]))).get() + 1));
                            } else{
                                keyWords.map.put(new Text(words[j]), new IntWritable(1));
                            }

                        }
                    }
                    context.write(new Text(words[i]), keyWords);
                }
            }
        }
    }

    // Replace "?" with your own key / value types
    // NOTE: combiner's output key / value types have to be the same as those of mapper
    /*public static class TextCombiner extends Reducer<?, ?, ?, ?> {
        public void reduce(Text key, Iterable<Tuple> tuples, Context context)
            throws IOException, InterruptedException
        {
            // Implementation of you combiner function
        }
    }*/

    // Replace "?" with your own input key / value types, i.e., the output
    // key / value types of your mapper function
    public static class TextReducer extends Reducer<Text, ContextMap, Text, Text> {
        private final static Text emptyText = new Text("");

        public void reduce(Text key, Iterable<ContextMap> queryMaps, Context context)
            throws IOException, InterruptedException
        {
            // Implementation of you reducer function

            ContextMap keyWords = new ContextMap();

            for(ContextMap keyWord : queryMaps){
                for(Writable queryWord : keyWord.map.keySet()){

                    Text word = (Text)queryWord;
                    IntWritable count = (IntWritable)keyWord.map.get(queryWord);

                    if(keyWords.map.containsKey(word)){ 
                        keyWords.map.put(word, new IntWritable(((IntWritable)keyWords.map.get(word)).get() + count.get()));
                    }
                    else{
                        keyWords.map.put(word, count);
                    }
        

                }
            }

            Map<Writable, Writable> map = keyWords.map;

            // sort the query words
            Writable[] queries = (Writable[]) keyWords.map.keySet().toArray(new Writable[keyWords.map.keySet().size()]);
            String[] queryWords = new String[queries.length];
            
            for(int i = 0; i<queryWords.length; i++){
                queryWords[i] = ((Text)queries[i]).toString();
            }
            Arrays.sort(queryWords);



            Text queryWordText = new Text();
            // Write out the results; you may change the following example
            // code to fit with your reducer function.
            //   Write out the current context key
            context.write(key, emptyText);
            //   Write out query words and their count
            for(String queryWord: queryWords){
                String count = ((IntWritable)map.get(new Text(queryWord))).toString() + ">";
                queryWordText.set("<" + queryWord + ",");
                context.write(queryWordText, new Text(count));
            }
            //   Empty line for ending the current context key
            context.write(emptyText, emptyText);
        }
    }

    public int run(String[] args) throws Exception {
        Configuration conf = this.getConf();

        // Create job
        Job job = new Job(conf, "ndc466_EID2"); // Replace with your EIDs
        job.setJarByClass(TextAnalyzer.class);

        // Setup MapReduce job
        job.setMapperClass(TextMapper.class);
        //   Uncomment the following line if you want to use Combiner class
        // job.setCombinerClass(TextCombiner.class);
        job.setReducerClass(TextReducer.class);

        // Specify key / value types (Don't change them for the purpose of this assignment)
        job.setOutputKeyClass(Text.class);
        job.setOutputValueClass(ContextMap.class);
        //   If your mapper and combiner's  output types are different from Text.class,
        //   then uncomment the following lines to specify the data types.
        //job.setMapOutputKeyClass(?.class);
        //job.setMapOutputValueClass(?.class);

        // Input
        FileInputFormat.addInputPath(job, new Path(args[0]));
        job.setInputFormatClass(TextInputFormat.class);

        // Output
        FileOutputFormat.setOutputPath(job, new Path(args[1]));
        job.setOutputFormatClass(TextOutputFormat.class);

        // Execute job and return status
        return job.waitForCompletion(true) ? 0 : 1;
    }

    // Do not modify the main method
    public static void main(String[] args) throws Exception {
        int res = ToolRunner.run(new Configuration(), new TextAnalyzer(), args);
        System.exit(res);
    }

    // You may define sub-classes here. Example:
    // public static class MyClass {
    //
    // }
    public static class ContextMap implements Writable {

        public Map<Writable, Writable> map;

        public ContextMap(){
            map = new MapWritable();
        }

        // need to overwrite Writable methods
        @Override
        public void write(DataOutput out) throws IOException{
            ((MapWritable)map).write(out);
        }        

        @Override
        public void readFields(DataInput in) throws IOException{
            ((MapWritable)map).readFields(in);
        }

        @Override
        public boolean equals(Object o)
        {
            if(o instanceof MapWritable)
            {
                return ((MapWritable)map).equals(o);
            }
        
            return false;
        }

        @Override
        public int hashCode(){
            return ((MapWritable)map).hashCode();
        }

     }
}