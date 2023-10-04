import sys
import csv
import urllib.response
import urllib.request

# check if the user supplied the correct number of command line arguments
if len(sys.argv) != 6:
    print("Usage: python titlegen.py inputfilename.csv  startlineno endlineno outputfilename.csv API_key")
    sys.exit(1)
    # set the api key from the command line argument
nonempty_artnr = 0
startlineno = int(sys.argv[2])
endlineno = int(sys.argv[3])
lineno = 0
api_key = sys.argv[5]
with open(sys.argv[1], "r") as file:
        outfile = open(sys.argv[4], "w")
        writer = csv.writer(outfile)
        reader = csv.reader(file)
        for row in reader:
            lineno += 1
            if (lineno < startlineno or lineno > endlineno):
                continue
            if lineno == 1:
                newrow = row
                row.append("Materials")
                row.append("Dimensions")
                row.append("Accession Number")
                writer.writerow(newrow)
                continue
                
            # get the 7th column of the row to a string variable
            itemurl = row[5]
            print(itemurl)
            # Extract the string after the last slash in the itemurl
            itemid = itemurl.rsplit('/', 1)[-1]
            print(itemid)
            print("====================================="   + str(lineno) + "=====================================")         
            # Fetch the xml data from "https://api.dimu.org/api/artifact?unique_id=<itemid>&mapping=ESE&api.key=<api_key"
            url = "https://api.dimu.org/api/artifact?unique_id=" + itemid + "&mapping=ESE&api.key=" + api_key
            print(url)
            response = urllib.request.urlopen(url)
            data = response.read()
            # Convert the xml data to a string
            data = data.decode("utf-8")
            # Extract the fields in the xml data that has are inside <dcterms:medium> tag as string variables
            
            # Use an xml parser and extract the fields in the xml data that has are inside <dcterms:medium> tag as string variables
            from xml.dom.minidom import parse, parseString  
            
            dom = parseString(data)
            
            # Extract the fields in the xml data that has are inside <dc:description> tag as string variables
            artnr = ""
            description_artnr = ""
            description_list = dom.getElementsByTagName ('dc:description') 
            # iterate through the list of description_list and find a line that starts with the string "Art.nr."
            for description in description_list:
                descriptionitem = description.firstChild.nodeValue
                # if "Art.nr." is a substring of the descriptionitem, then set description_artnr to descriptionitem and break
                if "Art.nr." in descriptionitem:
                    description_artnr = descriptionitem
                    # Store the integer value of the string after the string "Art.nr." in the variable artnr
                    artnr = description_artnr.split("Art.nr. ")[1]
                    # remove any terminating '.' from the string artnr
                    artnr = artnr.rstrip('.')
                    
                    print ("Found accn number and it is: ")
                    print(artnr)
                    break
                
           
       
            
            # Extract the fields in the xml data that has are inside <dcterms:medium> tag as string variables
            allmediums = ""
            medium_list = dom.getElementsByTagName('dcterms:medium')
            # iterate through the list of medium_list and print the value of the first child node of each element
            
            for medium in medium_list:
                mediumname = medium.firstChild.nodeValue
                allmediums += mediumname + " "
            print(allmediums)
            
            allextents = ""
            extent_list = dom.getElementsByTagName('dcterms:extent')
            # iterate through the list of extent_list and print the value of the first child node of each element
            for extent in extent_list:
                extentitem = extent.firstChild.nodeValue
                allextents += extentitem + " "
            print(allextents)
            
            if (artnr != ""):
                nonempty_artnr+=1
            
            # Generate a new row with the previosu row appended with the new fields medium and extent
            newrow = row
            newrow.append(allmediums)
            newrow.append(allextents)
            newrow.append(artnr)
            writer.writerow(newrow)
            
        outfile.close()
        file.close()
        print("Number of non-empty accession numbers: " + str(nonempty_artnr))
