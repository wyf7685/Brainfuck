# ++++++++++++++++++  # $0=18
# >++++<  # $1=4

# 计算 $0 % $1
>>+<<  # $2=true
>>[  # while $2
  [-]<<  # $2=false

  [->>>+>+<<<<]>>>>[-<<<<+>>>>]<<<<  # 复制$0到$3 : $4
  > [->>>+>+<<<<]>>>>[-<<<<+>>>>]<<<< <  # 复制$1到$4 : $5

  >>> ## 判断 $3<$4 开始
    # 缩进内$+3
    [->>+>+<<<]>>>[-<<<+>>>]<<<  # 复制$0到$2
    > [->>+>+<<<]>>>[-<<<+>>>]<<< <  # 复制$1到$3
    >>>>[-]>[-]+>[-]>[-]<<<<<<<  # $4=0, $5=1, $6=0, $7=0
    >>>>> [  # while $5
    [-] <<<<<  # $5=0
    
    >> [->>>>>+>+<<<<<<]>>>>>[-<<<<<+>>>>>]<<<<< <<  # 复制$2到$8 : $7
    >>>>>>>> [[-]<+>] <<<<<<<<  # $7=!!$8, $8=0
    
    >>>>>>>[- <<<<<<< # while $7
        ## if $3: $5=$6=1
        >>> [->+>>>>+<<<<<]>>>>>[-<<<<<+>>>>>]<<<<< <<<  # 复制$3到$4 : $8
        >>>>[  # if $4
        [-] # $4=0
        >[-]+  # $5=1
        >[-]+  # $6=1
        <<] <<<< # 判断复位$4
    
        ## if $2 && $3: $2--, $3-- 
        >>>>>>[  # if $6
        [-]  # $6=0
        <<<-  # $3--
        <-  # $2--
        >>>>] <<<<<< # 判断复位$6
    
        >> [->>>>>+>+<<<<<<]>>>>>[-<<<<<+>>>>>]<<<<< <<  # 复制$2到$8 : $7
        >>>>>>>> [[-]<+>] <<<<<<<<  # $7=!!$8, $8=0
        
        >>> [->+>>>>+<<<<<]>>>>>[-<<<<<+>>>>>]<<<<< <<<  # 复制$3到$4 : $8
        >>>>>>>>+<<<<[[-]>>>>-<<<<]<<<<  # $8=!$4, $4=0
        >>>>>>>> [[-]<[-]>] <<<<<<<<  # if $8: $7=$8=0
    
    >>>>>>>] <<<<<<<  # 循环复位$7
    >>>>>] <<<<<  # 循环复位$5
    
    >>>>[-]>[-]>[-]>[-]>[-]<<<<<<<<  # $4=$5=$6=$7=$8=0
    >>[-]>[[-]<+>]<<<  # 输出: $2 = ($0<$1)
    
    # 处理返回值
    [-]>[-]<  # $0=$1=0
  >>[-<<<+>>>]<<  # 判断结果移动到 $2
  <<<  ## 判断 $3<$4 结束

  >>>+<[[-]>-<]<<  # $3=!$2=($0>=$1), $2=0
  >>> [->+<<+>]>[-<+>]< <<<  # 复制$3到$2 : $4

  >>>[  # if $3 ($0>=$1)
    [-]<<<  # $3=false
    > [->>>+>+<<<<]>>>>[-<<<<+>>>>]<<<< <  # 复制$1到$4 : $5
    >>>>[-<<<<->>>>]<<<<  # $0-=$4, $4=0
  >>>] <<<  # 判断复位$3

>>] <<  # 循环复位$2
>[-]<  # $1=0, 取余结果为$0

# 输出(仅支持1位数值)
# >++++++[<++++++++>-]<.

%