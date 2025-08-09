-- AutoCodeGenerator.lua
-- ���ܣ�����AutoGenerate.txt�Զ���������������Replicate����������ʵ��

-- ================================================
-- ���ߺ�������ȡ������AutoGenerate.txt�еĹؼ���
-- ================================================
local function read_keywords(txt_path)
    local file = io.open(txt_path, "r")
    if not file then
        error("�����޷���AutoGenerate.txt�ļ�������·����")
    end
    
    local content = file:read("*a")  -- ��ȡȫ������
    file:close()
    
    -- �ָ�ŷָ��Ĺؼ��֣�ȥ����β�ո�
    local keywords = {}
    for kw in content:gmatch("[^,]+") do
        local trimmed = kw:gsub("^%s*(.-)%s*$", "%1")  -- ȥ��ǰ��ո�
        if trimmed ~= "" then  -- ���˿չؼ���
            table.insert(keywords, trimmed)
        end
    end
    return keywords
end

-- ================================================
-- ����.h�ļ���Ҫ��������������
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
-- ����.h�ļ���Ҫ��Replicate������������
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
-- ����.cpp�ļ���Ҫ��Replicate����ʵ�ִ���
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
-- ��.h�ļ��в������ɵ�����
-- ================================================
local function insert_into_header(h_path, property_code, func_decl_code)
    -- ��ȡԭʼ�ļ�����
    local file = io.open(h_path, "r")
    if not file then
        error("�����޷���ͷ�ļ�������·����")
    end
    local lines = {}
    for line in file:lines() do
        table.insert(lines, line)
    end
    file:close()

    -- ���Ҳ���λ��
    local prop_pos, func_pos = nil, nil
    for i, line in ipairs(lines) do
        if line:find("///HERE GENERATE PROPERTY") then
            prop_pos = i + 1  -- ���뵽����е���һ��
        elseif line:find("///HERE GENERATE FUNCTION") then
            func_pos = i + 1
        end
    end

    -- ִ�в������
    if prop_pos then
        table.insert(lines, prop_pos, property_code)
    else
        error("����ͷ�ļ���δ�ҵ�'///HERE GENERATE PROPERTY'��ǣ�")
    end

    if func_pos then
        table.insert(lines, func_pos, func_decl_code)
    else
        error("����ͷ�ļ���δ�ҵ�'///HERE GENERATE FUNCTION'��ǣ�")
    end

    -- д���޸ĺ������
    local file = io.open(h_path, "w")
    if not file then
        error("�����޷�д��ͷ�ļ�������Ȩ�ޣ�")
    end
    file:write(table.concat(lines, "\n"))  -- ����ԭ���з���ʽ
    file:close()
end

-- ================================================
-- ��.cpp�ļ��в������ɵ�����
-- ================================================
local function insert_into_source(cpp_path, func_impl_code)
    -- ��ȡԭʼ�ļ�����
    local file = io.open(cpp_path, "r")
    if not file then
        error("�����޷���Դ�ļ�������·����")
    end
    local lines = {}
    for line in file:lines() do
        table.insert(lines, line)
    end
    file:close()

    -- ���Ҳ���λ��
    local impl_pos = nil
    for i, line in ipairs(lines) do
        if line:find("///HERE GENERATE FUNCTION IMPLEMENTATION") then
            impl_pos = i + 1  -- ���뵽����е���һ��
        end
    end

    -- ִ�в������
    if impl_pos then
        table.insert(lines, impl_pos, func_impl_code)
    else
        error("����Դ�ļ���δ�ҵ�'///HERE GENERATE FUNCTION IMPLEMENTATION'��ǣ�")
    end

    -- д���޸ĺ������
    local file = io.open(cpp_path, "w")
    if not file then
        error("�����޷�д��Դ�ļ�������Ȩ�ޣ�")
    end
    file:write(table.concat(lines, "\n"))  -- ����ԭ���з���ʽ
    file:close()
end

-- ================================================
-- �����������̿���
-- ================================================
local function main()
    -- ���ò������û���Ҫ����ʵ������޸ģ�
    local config = {
        h_path = "OpenAttributeSet.h",  -- ͷ�ļ�·��
        cpp_path = "OpenAttributeSet.cpp",  -- Դ�ļ�·��
        txt_path = "AutoGenerate.txt"  -- �ؼ��������ļ�·��
    }

    -- ����1����ȡ�������ؼ���
    print("���ڶ�ȡ�ؼ�������...")
    local keywords = read_keywords(config.txt_path)
    print("�ҵ��ؼ��֣�" .. table.concat(keywords, ", "))

    -- ����2�����ɸ������Ƭ��
    print("�������ɴ���Ƭ��...")
    local property_code = generate_property_declarations(keywords)
    local func_decl_code = generate_replicate_declarations(keywords)
    local func_impl_code = generate_replicate_implementations(keywords)

    -- ����3�����뵽ͷ�ļ�
    print("������ͷ�ļ��������...")
    insert_into_header(config.h_path, property_code, func_decl_code)

    -- ����4�����뵽Դ�ļ�
    print("������Դ�ļ��������...")
    insert_into_source(config.cpp_path, func_impl_code)

    print("\n����������ɣ�")
    print("ͷ�ļ�·����" .. config.h_path)
    print("Դ�ļ�·����" .. config.cpp_path)
end

-- ִ��������
main()