import sys
import csv
import openai

def main():
    # check if the user supplied the correct number of command line arguments
    if len(sys.argv) != 3:
        print("Usage: python titlegen.py filename.csv API_key")
        sys.exit(1)
    #set the api key from the command line argument
    openai.api_key = sys.argv[2]
    with open(sys.argv[1], "r") as file:
        reader = csv.reader(file)
        for row in reader:
            print(row[4])
            # summarize the text in the 5th column using OpenAi's GPT-3
            # create a variable called prompt and set it as the concatenatenation of the string "Summarize this 
            # in one sentence:" and the text in the 5th column
            prompt = "Summarize this in one sentence:" + row[4]

            response = openai.Completion.create(
                model="gpt-3.5-turbo",
                prompt=prompt,
                temperature=0.7,
                max_tokens=64,
                top_p=1.0,
                frequency_penalty=0.0,
                presence_penalty=0.0
            )
            # write the response as the last column in each row of the same csv file
            row.append(response)
            print(row)
        


