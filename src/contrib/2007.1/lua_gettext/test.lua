-- load the library
foo = loadlib("./liblua_i18n.so", "luaopen_i18n")
foo()

-- this needs to be done!
os.setlocale("")
i18n.setenv("LANGUAGE", "de", 1) -- switch this!
i18n.bindtextdomain("messages", "locale")

msg = i18n.gettext("Hello") .. " " .. i18n.gettext("World")

print(msg)
print(i18n.gettext("native language"))

print(i18n.gettext("foobar"))
