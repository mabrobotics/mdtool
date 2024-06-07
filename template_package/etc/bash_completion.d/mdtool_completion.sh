#!/usr/bin/env bash

_have mdtool &&

_mdtool_complete()
{
	local cur prev
	local MEMO_DIR=$HOME/.config/mdtool/mdtool_motors
	
	COMPREPLY=()
	cur=${COMP_WORDS[COMP_CWORD]}
	prev=${COMP_WORDS[COMP_CWORD-1]}
	prev2=${COMP_WORDS[COMP_CWORD-2]}
	
	if [ $COMP_CWORD -eq 1 ]; then
		COMPREPLY=( $(compgen -W "blink bus clear config encoder ping register reset setup test" -- $cur) )
	elif [ $COMP_CWORD -eq 2 ]; then
		case "$prev" in
			"bus")
			COMPREPLY=( $(compgen -W "SPI UART USB" -- $cur) )
			;;
			"clear")
			COMPREPLY=( $(compgen -W "error warning" -- $cur) )
			;;
			"config")
			COMPREPLY=( $(compgen -W "bandwidth can current save zero" -- $cur) )
			;;
			"ping")
			COMPREPLY=( $(compgen -W "all" -- $cur) )
			;;
			"register")
			COMPREPLY=( $(compgen -W "read write" -- $cur) )
			;; 
			"setup")
			COMPREPLY=( $(compgen -W "calibration calibration_out homing info motor read_config" -- $cur) )
			;;
			"test")
			COMPREPLY=( $(compgen -W "encoder latency move" -- $cur) )
			;;
			*)
			;;
		esac
	elif [ $COMP_CWORD -eq 3 ]; then
		if [[ "$prev" == "encoder" ]]; then
			COMPREPLY=( $(compgen -W "main output" -- $cur) )

		fi
		if [[ "$prev" == "move" ]]; then
			COMPREPLY=( $(compgen -W "absolute" -- $cur) )

		fi

	elif [ $COMP_CWORD -eq 4 ]; then
		if [[ "$prev2" == "motor" ]]; then
			if [[ "$cur" == ./* || "$cur" == /* ]]; then
				# Autocomplete with files from the current directory
				_filedir '@(fit?(s)|cfg)'
			else
				local arr i file

				arr=( $( cd "$MEMO_DIR" && compgen -o nospace  -f -- "$cur" ) )
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
	fi
	
	return 0
} &&

complete -F _mdtool_complete mdtool
