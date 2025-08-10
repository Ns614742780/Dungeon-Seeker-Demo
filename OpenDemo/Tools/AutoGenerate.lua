-- AutoCodeGenerator.lua

local function read_keywords(txt_path)
    local file = io.open(txt_path, "r")
    
    local content = file:read("*a")
    file:close()
    
    local keywords = {}
    for kw in content:gmatch("[^,]+") do
        local trimmed = kw:gsub("^%s*(.-)%s*$", "%1")
        if trimmed ~= "" then
            table.insert(keywords, trimmed)
        end
    end
    return keywords
end

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

local function insert_into_header(h_path, property_code, func_decl_code)
    local file = io.open(h_path, "r")
    local lines = {}
    for line in file:lines() do
        table.insert(lines, line)
    end
    file:close()

    local prop_pos, func_pos = nil, nil
    for i, line in ipairs(lines) do
        if line:find("///HERE GENERATE PROPERTY") then
            prop_pos = i + 1
        elseif line:find("///HERE GENERATE FUNCTION") then
            func_pos = i + 1
        end
    end

    if prop_pos then
        table.insert(lines, prop_pos, property_code)
    end

    if func_pos then
        table.insert(lines, func_pos, func_decl_code)
    end

    local file = io.open(h_path, "w")
    file:write(table.concat(lines, "\n"))
    file:close()
end

local function insert_into_source(cpp_path, func_impl_code)
    local file = io.open(cpp_path, "r")
    local lines = {}
    for line in file:lines() do
        table.insert(lines, line)
    end
    file:close()

    local impl_pos = nil
    for i, line in ipairs(lines) do
        if line:find("///HERE GENERATE FUNCTION IMPLEMENTATION") then
            impl_pos = i + 1
        end
    end

    if impl_pos then
        table.insert(lines, impl_pos, func_impl_code)
    end

    local file = io.open(cpp_path, "w")
    file:write(table.concat(lines, "\n")) 
    file:close()
end

local function main()
    local config = {
        h_path = "OpenAttributeSet.h",
        cpp_path = "OpenAttributeSet.cpp",
        txt_path = "AutoGenerate.txt"
    }


    local keywords = read_keywords(config.txt_path)

    local property_code = generate_property_declarations(keywords)
    local func_decl_code = generate_replicate_declarations(keywords)
    local func_impl_code = generate_replicate_implementations(keywords)

    insert_into_header(config.h_path, property_code, func_decl_code)

    insert_into_source(config.cpp_path, func_impl_code)
end

main()