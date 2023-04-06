import sys
import csv
import openai

# check if the user supplied the correct number of command line arguments
if len(sys.argv) != 4:
    print("Usage: python titlegen.py inputfilename.csv outputfilename.csv API_key")
    sys.exit(1)
    # set the api key from the command line argument
openai.api_key = sys.argv[3]
with open(sys.argv[1], "r") as file:
        outfile = open(sys.argv[2], "w")
        writer = csv.writer(outfile)
        reader = csv.reader(file)
        for row in reader:
            print(row[4])
            # summarize the text in the 5th column using OpenAi's GPT-3
            # create a variable called prompt and set it as the concatenatenation of the string "Summarize this
            # in one sentence:" and the text in the 5th column
            prompt = "Summarize the following description of a museum item in one English phrase of strictly less than 5 words:" + row[4]

            completion = openai.ChatCompletion.create( # 1. Change the function Completion to ChatCompletion
                model = 'gpt-3.5-turbo',
                messages = [
                    {'role': 'user', 'content': prompt}
                ],
                temperature = 0  
            )

            # write the response as the last column in each row of the same csv file
            row.append(completion['choices'][0]['message']['content'])
            print(row)
            # write the row to the csv file

            writer.writerow(row)
        outfile.close()
        file.close()
