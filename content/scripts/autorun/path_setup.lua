-- assume lua >= 5.1 && package support
assert(type(package) == 'table' and type(package.path) == 'string')

-- append content/scripts to the search path
package.path = package.path .. ';content/scripts/?.lua'
