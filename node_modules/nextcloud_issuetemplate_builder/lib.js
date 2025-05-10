function renderSection(section) {
    var heading = '## ' + section.name
    var content = section.content
    return heading + '\n' + content
}

function IssueTemplateBuilder() {
    var sections = []

    return {
        addSection: function (name, content) {
            sections.push({
                name: name,
                content: content
            })

            return this
        },

        addEmptyStepsToReproduce: function () {
            this.addSection('Steps to reproduce', '1.\n2.\n3.')

            return this
        },

        addExpectedActualBehaviour: function () {
            this.addSection('Expected behaviour', 'Tell us what should happen')
            this.addSection('Actual behaviour', 'Tell us what happens instead')

            return this
        },

        addBrowserInfo: function (name, version) {
            if (!name) {
                name = browser.name
            }
            if (!version) {
                version = browser.version
            }
            this.addSection('Browser', name + ' ' + version)

            return this
        },

        addLogs: function (name, logs) {
            this.addSection(name, '```\n' + logs + '\n```')

            return this
        },

        render: function () {
            return sections.reduce(function (acc, section) {
                return acc + renderSection(section) + '\n\n'
            }, '').trim()
        }
    }
}

module.exports = IssueTemplateBuilder
