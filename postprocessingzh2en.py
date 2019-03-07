# coding=utf-8

import re
import sys
import time
import numpy
import Levenshtein


# en2zh
# python ...py en zh out

def RemoveSpaceToSmaoll(str):
    str = str.replace(" ", "")
    str = str.lower()
    return str


def find_lcseque(s1, s2):
    # 生成字符串长度加1的0矩阵，m用来保存对应位置匹配的结果
    m = [[0 for x in range(len(s2) + 1)] for y in range(len(s1) + 1)]
    # d用来记录转移方向
    d = [[None for x in range(len(s2) + 1)] for y in range(len(s1) + 1)]

    for p1 in range(len(s1)):
        for p2 in range(len(s2)):
            if s1[p1] == s2[p2]:  # 字符匹配成功，则该位置的值为左上方的值加1
                m[p1 + 1][p2 + 1] = m[p1][p2] + 1
                d[p1 + 1][p2 + 1] = 'ok'
            elif m[p1 + 1][p2] > m[p1][p2 + 1]:  # 左值大于上值，则该位置的值为左值，并标记回溯时的方向
                m[p1 + 1][p2 + 1] = m[p1 + 1][p2]
                d[p1 + 1][p2 + 1] = 'left'
            else:  # 上值大于左值，则该位置的值为上值，并标记方向up
                m[p1 + 1][p2 + 1] = m[p1][p2 + 1]
                d[p1 + 1][p2 + 1] = 'up'
    (p1, p2) = (len(s1), len(s2))
    # print numpy.array(d)
    s = []
    while m[p1][p2]:  # 不为None时
        c = d[p1][p2]
        if c == 'ok':  # 匹配成功，插入该字符，并向左上角找下一个
            s.append(s1[p1 - 1])
            p1 -= 1
            p2 -= 1
        if c == 'left':  # 根据标记，向左找下一个
            p2 -= 1
        if c == 'up':  # 根据标记，向上找下一个
            p1 -= 1
    s.reverse()
    return ''.join(s)


def find_n_sub_str(src, sub, pos, start):
    index = src.find(sub, start)
    if index != -1 and pos > 0:
        return find_n_sub_str(src, sub, pos - 1, index + 1)
    return index


def GetListOfStopWords(filepath):
    f_stop = open(filepath)
    try:
        f_stop_text = f_stop.read()
        # f_stop_text = unicode(f_stop_text, 'utf-8')
    finally:
        f_stop.close()
    f_stop_seg_list = f_stop_text.split('\n')
    # print(f_stop_seg_list)
    return f_stop_seg_list


if __name__ == '__main__':
    # file1 = open("en.txt","r",encoding='UTF-8')
    # file2 = open("ggzh_mt061.txt","r",encoding='UTF-8')
    # file1 = open("cwmt18-dev.beam12.alpha1.3.ensemble15.trans", "r", encoding='UTF-8')  # en
    # file1 = open("cwmt18-dev.ensemble_libei_4_10", "r", encoding='UTF-8')  # en
    # file1 = open("liout2.txt", "r", encoding='UTF-8')  # en


    # file1 = open("cwmt18-dev.ensemble_rerank_libei_jiang_4_12", "r", encoding='UTF-8')  # en 
    # file2 = open("cwmt-zh.dev.replace.token.del", "r", encoding='UTF-8')  # zh
    # file3 = open("liout2.txt", "w", encoding='UTF-8') # out
    file1 = open(sys.argv[1], "r", encoding='UTF-8')  # en 
    file2 = open(sys.argv[2], "r", encoding='UTF-8')  # zh
    file3 = open(sys.argv[3], "w", encoding='UTF-8') # out    
    file4 = open("dont.txt", "w", encoding='UTF-8')
    file5 = open("dont2.txt", "w", encoding='UTF-8')
    file_log = open("lozh2en 4 12", "a", encoding='UTF-8')
    file_log.write("\n***********************" + time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(
        time.time())) + "******************************\n\n")

    # 加载停用词表
    list_stopwords = GetListOfStopWords("ENstopwords891.txt")

    pattern_allsymbol = re.compile(r'[\s+\.\:\#\!\/_,$%^*)(+\"\']+|[+——！，。？、~@#￥%……&*（）]+')
    pattern = re.compile(r'[ 0-9A-Za-z.]+')
    pattern2 = re.compile(r'[A-Za-z]+')
    pattern_noten = re.compile(r'[^A-Za-z]+')
    pattern_num = re.compile(r'[0-9]+')
    pattern = re.compile(r'[^\u4e00-\u9fa5]+')

    count = 0
    count_dont = 0

    while True:
        line_en = file1.readline()
        line_zh = file2.readline()
        # if "看看 Mizrachi" in line_zh:
        #     print("aaa")
        # print(line_zh)
        # 测试
        # line_en = "But that does not prevent the industry from seeing the iPhone 7 's prospects - - which many believe will likely be the last big upgrade for Apple 's iPhone lineup . "
        # line_zh = "但 这 并 不 妨碍 业内人士 看好 iPhone7 的 前景 - - 许多 人 认为 , iPhone7 将 很 有 可能 成为 苹果 对 iPhone 系列 的 最后 一 次 大幅 升级 . "
        oldzh = line_zh
        if line_en == None or line_zh == None or len(line_en) == 0 or len(line_zh) == 0:
            break

        # 判断是否为包含英文
        ans2 = pattern2.findall(line_zh)
        if len(ans2) == 0:
            file3.write(line_en.strip() + "\n")
            continue

        flag = 1

        re_ans = pattern.findall(line_zh)  # 中文中的所有英文成分

        # 预处理 增加去掉符号的
        for item in re_ans:
            # item = item.strip()
            if len(item.strip()) == 0:
                re_ans.remove(item)
            else:
                # \' 单引号不去除
                item_sub = re.sub(r"[\.\!\/_,$%^:#\-!@#$%^&*)(+\"\']+|[+——！，。？、~@#￥%……&*（）]+", "", item)
                if item != item_sub:
                    re_ans.append(item_sub)
        # print(re_ans)

        for item in re_ans:
            # item 去掉首尾空格
            item = item.strip()
            # print(item)

            # 终止处理
            if len(item) == 0:
                continue
            if item == '.' or item[0] == '.':
                continue
            if item in line_en:
                continue

            # 完全去空格处理
            en_processed = RemoveSpaceToSmaoll(line_en)
            zh_processed = RemoveSpaceToSmaoll(item)

            if zh_processed in en_processed:  # 完全相同 大小写和空格问题 解决方法 定位，完全替换
                pos = 0
                start = 0
                subpos = find_n_sub_str(en_processed, zh_processed, pos, start)
                while subpos != -1:
                    temp_en = ""
                    start = subpos
                    tempsubpos = subpos
                    for i in line_en:
                        # 先tempsubpos定位到带个相同的位置 然后 获取
                        if tempsubpos == 0:
                            temp_en += i
                            # print("temp_en :" + temp_en)
                            if RemoveSpaceToSmaoll(temp_en) == zh_processed:
                                if temp_en.strip() != item.strip():
                                    # print("done!")
                                    file_log.write("temp_en : " + temp_en + "\n")
                                    file_log.write("item : " + item + "\n")
                                    file_log.write("zh:\t" + oldzh.strip() + "\n")
                                    file_log.write("olden:\t" + line_en.strip() + "\n")
                                    file_log.write("newen:\t" + line_en.replace(temp_en.strip(), item) + "\n" + "\n")
                                    line_en = line_en.replace(temp_en.strip(), item)
                                    count = count + 1
                                    # print(count)
                                    flag = 0
                                    break
                            continue
                        if i != ' ':
                            tempsubpos = tempsubpos - 1
                        else:
                            pass
                    pos = pos + 1
                    subpos = find_n_sub_str(en_processed, zh_processed, pos, start)
                    # print(subpos)

            else:
               # 这里有分词要求  可以选择使用
               # enitem item中的存英文 不包含标点之类的
               enitem = pattern2.findall(item)
            #    print(enitem,item)
               if len(enitem) == 0:
                   continue
               temp_line_en = line_en
               for itemword in enitem:
                   # print(item,itemword,len(find_lcseque(en_processed, RemoveSpaceToSmaoll(itemword))) / len(itemword),find_lcseque(en_processed, itemword))
                   if len(find_lcseque(en_processed, RemoveSpaceToSmaoll(itemword))) / len(
                           itemword) > 0.7 and itemword not in list_stopwords and itemword in line_zh.split():  # 这个数值待调整
                       # file_log.write("\nradio <= 0.7: \n")
                       # file_log.write(line_zh + "\n")
                       # file_log.write(line_en + "\n")
                       # file_log.write(item + "\n\n")
                    #    print(line_en, " itemword: " + itemword)
                       # 不处理包含数字的
                       if len(pattern_num.findall(itemword)) != 0:
                           # print("1")
                           continue
                       for word in temp_line_en.split():
                        #    print("2 " + word)
                           if word not in list_stopwords \
                                   and len(pattern2.findall(word)) != 0 \
                                   and len(pattern_num.findall(word)) == 0 \
                                   and len(pattern_noten.findall(word)) == 0 \
                                   and len(pattern_noten.findall(itemword)) == 0 \
                                   and RemoveSpaceToSmaoll(word) != RemoveSpaceToSmaoll(itemword) \
                                   :
                               # print("3" + word)
                               if Levenshtein.distance(RemoveSpaceToSmaoll(word), RemoveSpaceToSmaoll(itemword)) / len(itemword) < 0.3:
                                   # print("4")
                                   # print(word,itemword,item)
                                   # print(line_en, " itemword: " + itemword, line_zh,"item:" + item , re_ans)

                                   file_log.write("\nsimilar 0.7 0.3: \n")
                                   file_log.write("oldword:" + word + "  \n" + "newword:  " + itemword)
                                   file_log.write(
                                       "\nold: " + line_en + "\n" + "zh: "+ line_zh + "\n  item:" + item)
                                   line_en = line_en.replace(word, itemword)
                                   count = count + 1
                                   flag = 0
                                   file_log.write("\nnew: " + line_en + "\n")


            ''' 上一个方法 24.96'''
            # # 上一个方法 24.96
            # elif len(find_lcseque(en_processed, zh_processed)) / len(
            #                 zh_processed) > 0.7 and zh_processed not in list_stopwords:  # 这个数值待调整
            #             # file_log.write("\nradio <= 0.7: \n")
            #             # file_log.write(line_zh + "\n")
            #             # file_log.write(line_en + "\n")
            #             # file_log.write(item + "\n\n")
            #             # print(line_en, " zh_processed: " + zh_processed, line_zh,"item:" + item , re_ans)
            #             # 不处理包含数字的
            #             if len(pattern_num.findall(zh_processed)) != 0:
            #                 # print("1")
            #                 continue
            #             for word in line_en.split():
            #                 # print("2 " + word)
            #                 if word not in list_stopwords \
            #                         and len(pattern2.findall(word)) != 0 \
            #                         and len(pattern_num.findall(word)) == 0 \
            #                         and len(pattern_noten.findall(word)) == 0 \
            #                         and len(pattern_noten.findall(zh_processed)) == 0 \
            #                         and RemoveSpaceToSmaoll(word) != RemoveSpaceToSmaoll(zh_processed) \
            #                         :
            #                     print("3" + word)
            #                     if Levenshtein.distance(word, zh_processed) / len(zh_processed) < 0.3:
            #                         # print("4")
            #                         # print(word,zh_processed,item)
            #                         # print(line_en, " zh_processed: " + zh_processed, line_zh,"item:" + item , re_ans)
            #
            #                         file_log.write("\nsimilar 0.7 0.3: \n")
            #                         file_log.write("oldword:" + word + "  \n" + "newword:  " + zh_processed)
            #                         file_log.write(
            #                             "\nold: " + line_en + "\n" + "zh: "+ line_zh + "\n  item:" + item)
            #                         line_en = line_en.replace(word, item)
            #                         count = count + 1
            #                         flag = 0
            #                         file_log.write("\nnew: " + line_en + "\n")

        if flag == 0:
            flag = 1
        elif item not in line_en:
            file4.write(line_en.strip() + "\n\n")
            file4.write(line_zh.strip() + "\n")
            for i in re_ans:
                file4.write(i + " ")
            file4.write("\n")
            file4.write(str(len(find_lcseque(en_processed, zh_processed)) / len(zh_processed)))
            file4.write("\n")
            file4.write(str(Levenshtein.distance(word, zh_processed) / len(zh_processed)))
            file4.write("\n\n\n")
        else:
            file5.write(line_en.strip() + "\n")
            file5.write(line_zh.strip() + "\n\n")

        file3.write(line_en.strip() + "\n")
        # break
    print(count)
    print("Post Processing Done!")




 # else:
 #                # 这里有分词要求  可以选择使用
 #                # enitem item中的存英文 不包含标点之类的
 #                enitem = pattern2.findall(item)
 #                print(enitem)
 #                temp_line_en = line_en
 #                for itemword in enitem:
 #                    # print(item,itemword)
 #                    if len(find_lcseque(en_processed, itemword)) / len(
 #                            itemword) > 0.7 and itemword not in list_stopwords:  # 这个数值待调整
 #                        # file_log.write("\nradio <= 0.7: \n")
 #                        # file_log.write(line_zh + "\n")
 #                        # file_log.write(line_en + "\n")
 #                        # file_log.write(item + "\n\n")
 #                        # print(line_en, " itemword: " + itemword, line_zh,"item:" + item , re_ans)
 #                        # 不处理包含数字的
 #                        if len(pattern_num.findall(itemword)) != 0:
 #                            # print("1")
 #                            continue
 #                        for word in temp_line_en.split():
 #                            # print("2 " + word)
 #                            if word not in list_stopwords \
 #                                    and len(pattern2.findall(word)) != 0 \
 #                                    and len(pattern_num.findall(word)) == 0 \
 #                                    and len(pattern_noten.findall(word)) == 0 \
 #                                    and len(pattern_noten.findall(itemword)) == 0 \
 #                                    and RemoveSpaceToSmaoll(word) != RemoveSpaceToSmaoll(itemword) \
 #                                    :
 #                                print("3" + word)
 #                                if Levenshtein.distance(word, itemword) / len(itemword) < 0.3:
 #                                    # print("4")
 #                                    # print(word,itemword,item)
 #                                    # print(line_en, " itemword: " + itemword, line_zh,"item:" + item , re_ans)
 #
 #                                    file_log.write("\nsimilar 0.7 0.3: \n")
 #                                    file_log.write("oldword:" + word + "  \n" + "newword:  " + itemword)
 #                                    file_log.write(
 #                                        "\nold: " + line_en + "\n" + "zh: "+ line_zh + "\n  item:" + item)
 #                                    line_en = line_en.replace(word, itemword)
 #                                    count = count + 1
 #                                    flag = 0
 #                                    file_log.write("\nnew: " + line_en + "\n")
