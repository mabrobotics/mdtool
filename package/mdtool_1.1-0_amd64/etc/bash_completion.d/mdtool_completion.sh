#!/usr/bin/env bash

_have mdtool &&

_mdtool_complete()
{
	local cur prev
	
	COMPREPLY=()
	cur=${COMP_WORDS[COMP_CWORD]}
	prev=${COMP_WORDS[COMP_CWORD-1]}
	
	if [ $COMP_CWORD -eq 1 ]; then
		COMPREPLY=( $(compgen -W "bus ping config setup test blink encoder" -- $cur) )
	elif [ $COMP_CWORD -eq 2 ]; then
		case "$prev" in
			"bus")
			COMPREPLY=( $(compgen -W "USB SPI UART" -- $cur) )
			;;
			"ping")
			COMPREPLY=( $(compgen -W "all" -- $cur) )
			;;
			"config")
			COMPREPLY=( $(compgen -W "zero can save current" -- $cur) )
			;; 
			"setup")
			COMPREPLY=( $(compgen -W "calibration diagnostic" -- $cur) )
			;;
			*)
			;;
		esac
	fi
	
	return 0
} &&

complete -F _mdtool_complete mdtool