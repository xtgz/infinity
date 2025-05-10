let IssueTemplateBuilder = require('../lib')

describe('IssueTemplateBuilder', function () {
    it('should produce an empty template by default', function () {
        let builder = new IssueTemplateBuilder()

        var rendered = builder.render()

        expect(rendered).toBe('')
    })

    it('adds empty reproduction steps', function () {
        let builder = new IssueTemplateBuilder()

        var rendered = builder.addEmptyStepsToReproduce().render()

        expect(rendered).toBe(`## Steps to reproduce
1.
2.
3.`)
    })

    it('renders arbitrary sections', function () {
        let builder = new IssueTemplateBuilder()

        var rendered = builder.addSection('Test', 'content').render()

        expect(rendered).toBe('## Test\ncontent')
    })

    it('adds expected and actual behaviour', function () {
        let builder = new IssueTemplateBuilder()

        var rendered = builder.addExpectedActualBehaviour().render()

        expect(rendered).toBe(`## Expected behaviour
Tell us what should happen

## Actual behaviour
Tell us what happens instead`)
    })

    it('formats logs as code', function () {
        let builder = new IssueTemplateBuilder()

        var rendered = builder.addLogs('Nextcloud log', 'entry1\nentry2').render()

        expect(rendered).toBe(`## Nextcloud log
\`\`\`
entry1
entry2
\`\`\``)
    })

    it('detects the browser', function () {
        let builder = new IssueTemplateBuilder()

        var rendered = builder.addBrowserInfo('Firefox', '48').render()

        expect(rendered).toBe(`## Browser\nFirefox 48`)
    })
})
