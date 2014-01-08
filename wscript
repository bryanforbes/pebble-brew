
#
# This file is the default set of rules to compile a Pebble project.
#
# Feel free to customize this to your needs.
#

import re

top = '.'
out = 'build'

def options(ctx):
    ctx.load('pebble_sdk')

def configure(ctx):
    ctx.load('pebble_sdk')

def build(ctx):
    ctx.load('pebble_sdk')

    ctx.pbl_program(source=ctx.path.ant_glob('src/**/*.c'),
                    target='pebble-app.elf')

    config_html = ctx.path.find_node('src/js/config.html')
    config_css = ctx.path.find_node('src/js/config.css')

    config_css_txt = config_css.read()
    config_html_txt = config_html.read()

    # replace @import with CSS source
    config_html_txt = re.sub(r'^\s+@import "config.css";$', config_css_txt, config_html_txt, flags=re.M)

    # remove whitespace from the beginning of lines
    config_html_txt = re.sub(r'^\s+', '', config_html_txt, flags=re.M)
    # remove comments
    config_html_txt = re.sub(r'// .*$', '', config_html_txt, flags=re.M)
    config_html_txt = re.sub(r'/\*.*?\*/', '', config_html_txt, flags=re.S)
    # remove newlines
    config_html_txt = re.sub('(:?\r)?\n', '', config_html_txt)
    # replace ' with \'
    config_html_txt = re.sub("'", r"\\'", config_html_txt)

    src_js = ctx.path.make_node('src/js/pebble-js-app.js')
    build_js = ctx.path.get_bld().make_node('src/js/pebble-js-app.js')

    build_js.parent.mkdir()
    build_js.write(re.sub('__CONFIG_HTML__', config_html_txt, src_js.read()))

    # the following updates the node signature for waf
    ctx(rule='touch ${TGT}', target=build_js, update_outputs=True)

    ctx.pbl_bundle(elf='pebble-app.elf', js=build_js)
