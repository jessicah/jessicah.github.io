#!/usr/bin/env python3

import requests
from metaname import Client as Metaname

metaname = Metaname('https://metaname.net/api/1.1', REF, API_KEY)

matching_domains = {
	'haiku.nz': ['@', 'build', 'ports-mirror', 'dev', 'cgit', 'discuss', 'review', 'i18n', 'git', 'cdn'],
	'jesikat.nz': ['@', 'ha'],
	'jessicah.nz': ['cloud', 'jesika']
}

all_zones = [zone['name'] for zone in metaname.dns_zones()]

new_ip_address = '202.180.104.127'

for domain, subDomains in matching_domains.items():
	if not domain in all_zones:
		print(f'Skipping DNS Zone {domain}')
		continue
	
	for record in metaname.dns_zone(domain):
		name = record['name']
		
		if record['type'] != 'A':
			#print(f'Skipping {name}, is not an A record')
			continue
		
		if not name in subDomains and (name == domain + '.' and not '@' in subDomains):
			#print(f'Skipping {name}, not present in matching domains')
			continue
		
		print(f'Updating {name} record ...')
		
		reference = record.pop('reference')
		for k in record.copy().keys():
			if k not in ['type', 'data', 'name']:
				record.pop(k)
		record['data'] = new_ip_address
		metaname.update_dns_record(domain, reference, record)
