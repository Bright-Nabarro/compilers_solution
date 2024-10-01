# `yaml`文法书写规则
1. 示例[example.yaml](./test/test_yaml/example.yaml)
2. 产生式右部终结符和非终结符单引号必须添加
3. 特别注意产生右部单引号针对单个符号

# `grammar`完整构建顺序
1. `set_start` -- `add_rule`+  (两个方法没有强制规定先后顺序) 此时能够确保`get_start_unchecked`能够正常调用
2. `validate_noterminals` 此时能够确保 `display_latex`, `find`能够正常调用
3. `infer_empty_string` 此时能够确保所有观察器能够正常调用

