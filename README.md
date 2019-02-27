# postprocessing_Space-Name
Aiming at the situation that the target language words (such as person name and brand name) in the source text appears morphological error or translation error after translation

包括空格、大小写错误还原以及错误翻译还原（超参数控制）

e.g.1
> iPhone XS  -> iPhoneXs


e.g.2
>src: 几乎同一时间，亚马逊宣布旗下无人超市（Amazon Go）试运营

>trans: Almost at the same time , Amazon announced that its AmazonGGo trial operation 

>postp: Almost at the same time , Amazon announced that its Amazon Go trial operation 

18年7月完成

想法类似于copynet

优点：
- 轻量级
- 快速
- 参数可控
- 无需学习
- 可移植性好
