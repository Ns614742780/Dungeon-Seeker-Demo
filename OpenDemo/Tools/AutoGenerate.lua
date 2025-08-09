-- AutoCodeGenerator.lua
-- 功能：根据AutoGenerate.txt自动生成属性声明、Replicate函数声明及实现

-- ================================================
-- 工具函数：读取并解析AutoGenerate.txt中的关键字
-- ================================================
local function read_keywords(txt_path)
    local file = io.open(txt_path, "r")
    if not file then
        error("错误：无法打开AutoGenerate.txt文件，请检查路径！")
    end
    
    local content = file:read("*a")  -- 读取全部内容
    file:close()
    
    -- 分割逗号分隔的关键字（去除首尾空格）
    local keywords = {}
    for kw in content:gmatch("[^,]+") do
        local trimmed = kw:gsub("^%s*(.-)%s*$", "%1")  -- 去除前后空格
        if trimmed ~= "" then  -- 过滤空关键字
            table.insert(keywords, trimmed)
        end
    end
    return keywords
end

-- ================================================
-- 生成.h文件需要的属性声明代码
-- ================================================
local function generate_property_declarations(keywords)
    local declarations = {}
    for _, kw in ipairs(keywords) do
        table.insert(declarations, string.format(
            "UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_%s, Category = \"Secondary Attributes\")\nFGameplayAttributeData %s;\nATTRIBUTE_ACCESSORS(UOpenAttributeSet, %s)\n",
            kw, kw, kw
        ))
    end
    return table.concat(declarations)
end

-- ================================================
-- 生成.h文件需要的Replicate函数声明代码
-- ================================================
local function generate_replicate_declarations(keywords)
    local declarations = {}
    for _, kw in ipairs(keywords) do
        table.insert(declarations, string.format(
            "UFUNCTION()\nvoid OnRep_%s(const FGameplayAttributeData& Old%s) const;\n",
            kw, kw
        ))
    end
    return table.concat(declarations)
end

-- ================================================
-- 生成.cpp文件需要的Replicate函数实现代码
-- ================================================
local function generate_replicate_implementations(keywords)
    local implementations = {}
    for _, kw in ipairs(keywords) do
        table.insert(implementations, string.format(
            "void UOpenAttributeSet::OnRep_%s(const FGameplayAttributeData& Old%s) const\n{\n\tGAMEPLAYATTRIBUTE_REPNOTIFY(UOpenAttributeSet, %s, Old%s);\n}\n",
            kw, kw, kw, kw
        ))
    end
    return table.concat(implementations)
end

-- ================================================
-- 在.h文件中插入生成的内容
-- ================================================
local function insert_into_header(h_path, property_code, func_decl_code)
    -- 读取原始文件内容
    local file = io.open(h_path, "r")
    if not file then
        error("错误：无法打开头文件，请检查路径！")
    end
    local lines = {}
    for line in file:lines() do
        table.insert(lines, line)
    end
    file:close()

    -- 查找插入位置
    local prop_pos, func_pos = nil, nil
    for i, line in ipairs(lines) do
        if line:find("///HERE GENERATE PROPERTY") then
            prop_pos = i + 1  -- 插入到标记行的下一行
        elseif line:find("///HERE GENERATE FUNCTION") then
            func_pos = i + 1
        end
    end

    -- 执行插入操作
    if prop_pos then
        table.insert(lines, prop_pos, property_code)
    else
        error("错误：头文件中未找到'///HERE GENERATE PROPERTY'标记！")
    end

    if func_pos then
        table.insert(lines, func_pos, func_decl_code)
    else
        error("错误：头文件中未找到'///HERE GENERATE FUNCTION'标记！")
    end

    -- 写回修改后的内容
    local file = io.open(h_path, "w")
    if not file then
        error("错误：无法写入头文件，请检查权限！")
    end
    file:write(table.concat(lines, "\n"))  -- 保持原换行符格式
    file:close()
end

-- ================================================
-- 在.cpp文件中插入生成的内容
-- ================================================
local function insert_into_source(cpp_path, func_impl_code)
    -- 读取原始文件内容
    local file = io.open(cpp_path, "r")
    if not file then
        error("错误：无法打开源文件，请检查路径！")
    end
    local lines = {}
    for line in file:lines() do
        table.insert(lines, line)
    end
    file:close()

    -- 查找插入位置
    local impl_pos = nil
    for i, line in ipairs(lines) do
        if line:find("///HERE GENERATE FUNCTION IMPLEMENTATION") then
            impl_pos = i + 1  -- 插入到标记行的下一行
        end
    end

    -- 执行插入操作
    if impl_pos then
        table.insert(lines, impl_pos, func_impl_code)
    else
        error("错误：源文件中未找到'///HERE GENERATE FUNCTION IMPLEMENTATION'标记！")
    end

    -- 写回修改后的内容
    local file = io.open(cpp_path, "w")
    if not file then
        error("错误：无法写入源文件，请检查权限！")
    end
    file:write(table.concat(lines, "\n"))  -- 保持原换行符格式
    file:close()
end

-- ================================================
-- 主函数：流程控制
-- ================================================
local function main()
    -- 配置参数（用户需要根据实际情况修改）
    local config = {
        h_path = "OpenAttributeSet.h",  -- 头文件路径
        cpp_path = "OpenAttributeSet.cpp",  -- 源文件路径
        txt_path = "AutoGenerate.txt"  -- 关键字配置文件路径
    }

    -- 步骤1：读取并解析关键字
    print("正在读取关键字配置...")
    local keywords = read_keywords(config.txt_path)
    print("找到关键字：" .. table.concat(keywords, ", "))

    -- 步骤2：生成各类代码片段
    print("正在生成代码片段...")
    local property_code = generate_property_declarations(keywords)
    local func_decl_code = generate_replicate_declarations(keywords)
    local func_impl_code = generate_replicate_implementations(keywords)

    -- 步骤3：插入到头文件
    print("正在向头文件插入代码...")
    insert_into_header(config.h_path, property_code, func_decl_code)

    -- 步骤4：插入到源文件
    print("正在向源文件插入代码...")
    insert_into_source(config.cpp_path, func_impl_code)

    print("\n代码生成完成！")
    print("头文件路径：" .. config.h_path)
    print("源文件路径：" .. config.cpp_path)
end

-- 执行主函数
main()