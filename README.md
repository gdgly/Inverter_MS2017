# Inverter_MS2019
2017年盟升杯逆变器代码<br>
使用MSP430F5529软件环路，控制一路反激辅助电源和一路SPWM输出<br>
输出有稳幅和自动预失真，带感性载波形质量基本没有下降。试着带了一个节能灯这种整流性负载，波形甚至也可以修正回来<br>
环路调节速度比较慢，有优化空间（可能FFT点数太多了），长期使用会不会因为误差乱掉不太清楚，至少测评时没有出问题<br>
FFT库跟TI原本的不一样，做了修改，原来的库会跟编译器产生的硬件乘法操作冲突导致有中断情况下结果不对<br>