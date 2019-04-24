import sys
import re

print("python3 detoken.py input_filename")
print("output: input_filename.detoken")

filename = sys.argv[1]
ifile = open(filename, 'r', encoding='utf-8')
lines = ifile.readlines()
ofile = open(filename + ".detoken", 'w',encoding="UTF-8")

filtrate = re.compile(u'[\u4E00-\u9FA5“”]')
for line in lines:
	# NormalNum
	# line = normalNum(line)
	#line = line.strip()
	# space
	pattern_space = re.compile(r'[^a-zA-Z] [^a-zA-Z] ?')
	re_space = pattern_space.findall(line)
	for item_space in re_space:
		line = line.replace(item_space,item_space.replace(" ",""))
		item_space.replace(" ","")
		line = line.replace(item_space,item_space.replace(" ",""))

	pattern_space = re.compile(r' [^a-zA-Z]')
	re_space = pattern_space.findall(line)
	for item_space in re_space:
		line = line.replace(item_space,item_space.replace(" ",""))

	pattern_space = re.compile(r'[^a-zA-Z] ')
	re_space = pattern_space.findall(line)
	for item_space in re_space:
		line = line.replace(item_space,item_space.replace(" ",""))
	
	# point
	line = list(line)
	for index in  range(len(line)):
		if index == 0:
			continue
		if line[index] == "." and filtrate.match(line[index - 1]) != None:
			line[index] = '。'
	if len(line) >= 1 and line[-2] == ".":
		line[-2] = "。"
	line = ''.join(line)
	
	# Semi
	line = line.replace(";", "；") 
	
	# Even single quotes
	quote = re.findall(r"\'.+?\'",line)
	if len(quote) != 0:
		for item in quote:
			line = line.replace(item,item.replace("'","‘",1))
			item = item.replace("'","‘",1)
			line = line.replace(item,item.replace("'","’",1))  
			item = item.replace("'","’",1)              
	# Even double quotes
	quote_double = re.findall(r"\".+?\"",line)
	if len(quote_double) != 0:
		for item in quote_double:
			line = line.replace(item,item.replace("\"","“",1))
			# print(item)
			item = item.replace("\"","“",1)
			line = line.replace(item,item.replace("\"","”",1))  
			item = item.replace("\"","”",1)             
	# Odd double quotes 1
	quote_rear = re.findall(r"\"$",line)
	if len(quote_rear) != 0:
		for item in quote_rear:
			line = line.replace(item,item.replace("\"","”",1))
			item = item.replace("\"","”",1)            
	# Odd double quotes 2
	line = line.replace("\"","“")

	# Colon
	colon = re.findall(r"[^\d]:",line)           
	if len(colon) != 0:
		for item in colon:
			line = line.replace(item,item.replace(":","："))
			item = item.replace(":","：") 
	# Colon
	comma = re.findall(r"[^\d],",line)           
	if len(comma) != 0:
		for item in comma:
			line = line.replace(item,item.replace(",","，"))
			item = item.replace(",","，") 
	# other

	line = line.replace("(","（")
	line = line.replace(")","）")         
	line = line.replace("•","·")
	line = line.replace("!","！")
	line = line.replace("?","？")   
#	pattern_all = re.compile(r'\(.*?\)')
#	pattern_capital = re.compile(r'\([A-Z0-9\- ]+\)')          
#	re_all = pattern_all.findall(line)
#	re_capital = pattern_capital.findall(line)
	ofile.write(line)

ifile.close()
ofile.flush()
ofile.close()


#		if line[index] == "。":
#			if (line[index - 1].isalpha() or line[index - 1].isdigit()) and (line[index + 1].isalpha() or line[index + 1].isdigit()):
#				line[index] == "."

	# Decimal point
#	dot = re.findall(r"\d\. \d",line)    
#	if len(dot) != 0:
#		for item in dot:
#			line = line.replace(item,item.replace(". ","."))
#			item = item.replace(". ",".")   
