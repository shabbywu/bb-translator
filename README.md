# bb-translator
bb-translator 是 Battle Brothers(战场兄弟) 的文本翻译器, 翻译器使用[本地化项目](https://github.com/shabbywu/Battle-Brothers-CN)的词条文件翻译 data 目录下的游戏脚本。


主要特性:
- 使用 physfs 读取文件, 实现了与游戏同样的加载方式和顺序
- 同时支持翻译 .nut/.cnut, 理论上支持翻译 Mod(只要 词条文件 中有对应的汉化文本)
- 可汉化历史版本的游戏脚本(只要 词条文件 中有对应的汉化文本)
- GUI 界面

## 使用说明
1. 从 [Release](https://github.com/BattleBrothersGameCN/Release/releases) 下载最新版本的 bb-translator
2. 解压到游戏目录
3. 初次使用需要在联网环境下执行

> Q: 为什么要联网?
>
> A: 初次使用时, 需要联网初始化本地化项目 [Battle-Brothers-CN](https://github.com/shabbywu/Battle-Brothers-CN), 否则会因为缺失词条文件而无法使用。
>
> 此外, 建议定期更新本地化项目，因为我们的汉化成员会不定期校对已翻译的文本。

## 维护路线图
- [x] 能配合[本地化项目](https://github.com/shabbywu/Battle-Brothers-CN)正常汉化战场兄弟原版游戏
- [x] 实现了与游戏同样的 Mod 加载方式和顺序
- [ ] 支持自定义 Mod 加载顺序！
