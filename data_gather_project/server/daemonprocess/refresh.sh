#!/bin/bash
kill -SIGUSR1 `ps au | grep "\bserver\b" | awk '{print $2}'`
