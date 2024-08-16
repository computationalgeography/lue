# Keep these settings in sync with .codacy.yaml

# https://github.com/markdownlint/markdownlint/blob/main/docs/configuration.md
# https://github.com/markdownlint/markdownlint/blob/main/docs/RULES.md
all
exclude_rule 'MD004'
exclude_rule 'MD005'
exclude_rule 'MD012'
exclude_rule 'MD024'
exclude_rule 'MD030'
exclude_rule 'MD034'
exclude_rule 'MD041'
exclude_rule 'MD046'

# The parameters which can be changed are mentioned in the rules page (Parameters: ...)
rule 'MD007', :indent => 4
rule 'MD013', :line_length => 110
