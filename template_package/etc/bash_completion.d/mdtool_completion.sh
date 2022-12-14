#!/usr/bin/env bash

_have mdtool &&

_mdtool_complete()
{
	local cur prev prev2
	local MEMO_DIR=$HOME/.config/mdtool/mdtool_motors
	
	COMPREPLY=()
	cur=${COMP_WORDS[COMP_CWORD]}
	prev=${COMP_WORDS[COMP_CWORD-1]}
	prev2=${COMP_WORDS[COMP_CWORD-2]}
	
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
			COMPREPLY=( $(compgen -W "zero can save current bandwidth" -- $cur) )
			;; 
			"setup")
			COMPREPLY=( $(compgen -W "calibration diagnostic motor info" -- $cur) )
			;;
			"test")
			COMPREPLY=( $(compgen -W "move latency" -- $cur) )
			;;
			*)
			;;
		esac
	elif [ $COMP_CWORD -eq 4 ]; then
		if [[ "$prev2" == "motor" ]]; then

			local arr i file

			arr=( $( cd "$MEMO_DIR" && compgen -f -- "$cur" ) )
			COMPREPLY=()
			for ((i = 0; i < ${#arr[@]}; ++i)); do
				file=${arr[i]}
				if [[ -d $MEMO_DIR/$file ]]; then
					file=$file/
				fi
				COMPREPLY[i]=$file
			done
		fi
	fi
	
	return 0
} &&

complete -F _mdtool_complete mdtool
