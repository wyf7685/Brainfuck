# 函数: 计算 $0 / $1
>>[-]<<  # $2=0
>>>[-]+<<<  # $3=true
>>>[  # while $3
  [-]<<<  # $3=false
  [->>>>+>+<<<<<]>>>>>[-<<<<<+>>>>>]<<<<<  # 复制$0到$4 : $5
  > [->>>>+>+<<<<<]>>>>>[-<<<<<+>>>>>]<<<<< <  # 复制$1到$5 : $6

  >>>> ## 判断 $4<$5 开始
    # 缩进内$+4
    {bf/less_than_v2.bf}  # 调用函数 less_than
    # 输出: $0 = ($0<$1)
  <<<< ## 判断 $4<$5 结束, 结果为$4
  >>>> >+<[[-]>-<] <<<<  # $5=!$4=($0>=$1), $4=0
  >>>>> [-<+<+>>] <<<<<  # 复制$5到$3,$4, $5=0

  >>>>[  # if $4
    [-]<<<<  # $4=false
    > [->>>>+>+<<<<<]>>>>>[-<<<<<+>>>>>]<<<<< <  # 复制$1到$5 : $6
    >>>>> [-<<<<<->>>>>] <<<<<  # $0-=$5, $5=0
    >>+<<  # $2++
  >>>>] <<<< # 判断复位$4

>>>] <<< # 循环复位$3
# 计算结束, 商为 $2, 余为 $0