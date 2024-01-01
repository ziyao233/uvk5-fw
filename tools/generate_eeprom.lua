#!/usr/bin/env lua5.4

--[[
--	uvk5-fw
--	/tools/generate_eeprom.lua
--	This file is distributed under Apache License Version 2.0
--	Copyright (c) 2024 Yao Zi. All rights reserved.
--]]

local string		= require "string";
local io		= require "io";
local table		= require "table";

local cMaxSateliteNum <const> = 72;

--[[
--	Satelite Data File Format:
--		array [
			object {
				string	name (length <= 5)
				boolean	v2u (U to V transponder)
				string	CTCSS (CTCSS Frequency)
				int	uFreq	(U Frequnecy Limitation in Hz)
				int	vFreq	(V Frequency Limitation in Hz)
				array	slices [
						int, eight or less time slices
					       ]
			       }
		      ]
--]]
local satelites = assert(dofile(assert(arg[1], "No input specified")));
assert(#satelites < cMaxSateliteNum, "Too many data entries");
local output = assert(io.open(arg[2], "w"));

local ctcssReverted <const> = { "67.0", "69.3", "71.9", "74.4" };
local ctcss = {};
for i, v in pairs(ctcssReverted)
do
	ctcss[v] = i;
end
ctcss[""] = 0;

--[[
--	| name (5B) | attr (1B) | U Frequency (4B) | V Frequency (4B) |
--	| Time Slices (1B * 8, one unit stands for 4 seconds) |
--
--	attr:
--		| unused (2b) | direction (0 for v to u) | CTCSS (0 for none) |
--		7	      5				 4		      0
--]]
local function convert(s)
	assert(#s.name <= 5);
	local t = ("<c5I1I4I4"):pack(s.name .. (" "):rep(5 - #s.name),
				     (s.v2u and 1 or 0) << 4 | ctcss[s.CTCSS or ""],
				     s.uFreq, s.vFreq);
	for i = 1, 8
	do
		local length = s.slices[i] or 0;
		t = t .. ("<I1"):pack(length // 4);
	end
	return t;
end

local builder = {};
for i, v in ipairs(satelites)
do
	builder[i] = convert(v);
end

output:write(table.concat(builder));
output:write(("\xff"):rep(22));
output:close();
