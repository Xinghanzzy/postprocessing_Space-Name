import sys
filename = sys.argv[1]
ifile = open(filename, 'r', encoding='utf-8')
lines = ifile.readlines()
ofile = open(filename + ".detoken", 'w',encoding="UTF-8")

for line in lines:
	# NormalNum
	# line = normalNum(line)
	# Semi
	line = line.replace(";", "；") 
	line = line.replace(" .", " 。")

	# Decimal point
	dot = re.findall(r"\d\. \d",line)    
	if len(dot) != 0:
		for item in dot:
			line = line.replace(item,item.replace(". ","."))
			item = item.replace(". ",".")        
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
	pattern_all = re.compile(r'\(.*?\)')
	pattern_capital = re.compile(r'\([A-Z0-9\- ]+\)')          
	re_all = pattern_all.findall(line)
	re_capital = pattern_capital.findall(line)
	ofile.write(line)

ifile.close()
ofile.flush()
ofile.close()
