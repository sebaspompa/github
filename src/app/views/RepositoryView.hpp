#pragma once

#include <string>
#include <vector>

#include "src/engine/rendering/Canvas.hpp"
#include "src/engine/input/Event.hpp"
#include "src/engine/navigation/NavigationStack.hpp"

#include "src/engine/core/Point.hpp"
#include "src/engine/core/Rect.hpp"
#include "src/engine/terminal/ansi.hpp"

#include "src/app/viewmodels/RepositoryViewModel.hpp"

#include "src/engine/ui/Button.hpp"
#include "src/engine/ui/Link.hpp"
#include "src/engine/ui/InputField.hpp"

using std::size_t;
using std::string;
using std::vector;

struct RepositoryView
{
private:
    enum class Tab
    {
        Code,
        Commits,
        Issues,
        PullRequest
    };

    enum class Form
    {
        None,
        Issue,
        Branch,
        Directory,
        File,
        Commit,
        PullRequest
    };

    RepositoryViewModel repositoryVM;

    Tab currentTab = Tab::Code;
    Form currentForm = Form::None;

    IssueQuery issueQuery;
    PullRequestQuery pullRequestQuery;

    Issue::Priority newIssuePriority = Issue::Priority::High;

    PullRequest::Priority newPullRequestPriority = PullRequest::Priority::High;

    size_t newPullRequestSourceBranch = 0;
    size_t newPullRequestTargetBranch = 0;

    vector<Button> tabButtons;
    vector<Button> branchButtons;

    vector<Link> entryLinks;

    Link createBranchLink;
    Link createCommitLink;
    Link createFolderLink;
    Link createFileLink;

    Link priorityFilterLink;
    Link stateFilterLink;
    Link sortPriorityLink;
    Link createIssueLink;

    Link formPriorityLink;
    Link formCreateLink;
    Link formCancelLink;

    Link prPriorityFilterLink;
    Link prStateFilterLink;
    Link prSortPriorityLink;
    Link createPullRequestLink;

    InputField issueTitleField;
    InputField issueDescriptionField;

    InputField commitTitleField;
    InputField commitDescriptionField;

    InputField pullRequestTitleField;
    InputField pullRequestDescriptionField;

    Link prSourceBranchLink;
    Link prTargetBranchLink;
    Link prFormPriorityLink;
    Link prFormCreateLink;
    Link prFormCancelLink;

    InputField creationNameField;

    ANSI::Style tabNormalStyle;
    ANSI::Style tabHoverStyle;
    ANSI::Style tabActiveStyle;

    ANSI::Style branchNormalStyle;
    ANSI::Style branchHoverStyle;
    ANSI::Style branchActiveStyle;

    ANSI::Style entryNormalStyle;
    ANSI::Style entryHoverStyle;

    ANSI::Style sectionStyle;
    ANSI::Style frameStyle;

    void initStyles()
    {
        tabNormalStyle.foreground = ANSI::Color::White;

        tabHoverStyle.foreground = ANSI::Color::BrightMagenta;
        tabHoverStyle.bold = true;

        tabActiveStyle.foreground = ANSI::Color::BrightMagenta;
        tabActiveStyle.bold = true;
        tabActiveStyle.underline = true;

        branchNormalStyle.foreground = ANSI::Color::Green;

        branchHoverStyle.foreground = ANSI::Color::BrightGreen;
        branchHoverStyle.bold = true;

        branchActiveStyle.foreground = ANSI::Color::BrightGreen;
        branchActiveStyle.bold = true;

        entryNormalStyle.foreground = ANSI::Color::White;

        entryHoverStyle.foreground = ANSI::Color::BrightMagenta;
        entryHoverStyle.background = ANSI::Background::Magenta;
        entryHoverStyle.bold = true;

        sectionStyle.foreground = ANSI::Color::BrightMagenta;
        sectionStyle.bold = true;

        frameStyle.foreground = ANSI::Color::BrightBlack;
    }

    void initTabs()
    {
        tabButtons.clear();

        tabButtons.emplace_back(Button("CODE", Point(0, 1), tabActiveStyle, tabActiveStyle));

        tabButtons.emplace_back(Button("COMMITS", Point(0, 1), tabNormalStyle, tabHoverStyle));

        tabButtons.emplace_back(Button("ISSUES", Point(0, 1), tabNormalStyle, tabHoverStyle));

        tabButtons.emplace_back(Button("PULL REQUEST", Point(0, 1), tabNormalStyle, tabHoverStyle));
    }

    void initBranches()
    {
        branchButtons.clear();

        const vector<Branch> branches = repositoryVM.getBranches();

        for (const Branch &branch : branches)
        {
            branchButtons.emplace_back(Button(branch.name, Point(0, 0), branchNormalStyle, branchHoverStyle));
        }

        createBranchLink = Link("+ NEW BRANCH", "create-branch");

        createCommitLink = Link("+ NEW COMMIT", "create-commit");

        createFolderLink = Link("+ FOLDER", "create-folder");

        createFileLink = Link("+ FILE", "create-file");
    }

    void initEntries()
    {
        entryLinks.clear();

        for (const Entry &entry : repositoryVM.getEntries())
        {
            string icon;

            if (entry.type == Entry::Type::Directory)
            {
                icon = "▸ ";
            }
            else
            {
                icon = "• ";
            }

            entryLinks.emplace_back(Link(icon + entry.name, entry.name));
        }
    }

    void initIssues()
    {
        priorityFilterLink = Link("PRIORITY: ALL", "priority-filter");

        stateFilterLink = Link("STATE: ALL", "state-filter");

        sortPriorityLink = Link("SORT: HIGH -> LOW", "sort-priority");

        createIssueLink = Link("+ NEW ISSUE", "create-issue");
    }

    void initPullRequests()
    {
        prPriorityFilterLink = Link("PRIORITY: ALL", "pr-priority-filter");

        prStateFilterLink = Link("STATE: ALL", "pr-state-filter");

        prSortPriorityLink = Link("SORT: HIGH -> LOW", "pr-sort-priority");

        createPullRequestLink = Link("+ NEW PULL REQUEST", "create-pull-request");
    }

    void initIssueForm()
    {
        issueTitleField = InputField("title", "Title", 100, 50);

        issueDescriptionField = InputField("description", "Description", 250, 50);

        formPriorityLink = Link("PRIORITY: HIGH", "form-priority");

        formCreateLink = Link("CREATE", "form-create");

        formCancelLink = Link("CANCEL", "form-cancel");
    }

    void initCommitForm()
    {
        commitTitleField = InputField("title", "Title", 100, 50);

        commitDescriptionField = InputField("description", "Description", 250, 50);
    }

    void initPullRequestForm()
    {
        pullRequestTitleField = InputField("title", "Title", 100, 50);

        pullRequestDescriptionField = InputField("description", "Description", 250, 50);

        prSourceBranchLink = Link("SOURCE: NONE", "pr-source-branch");

        prTargetBranchLink = Link("TARGET: NONE", "pr-target-branch");

        prFormPriorityLink = Link("PRIORITY: HIGH", "pr-form-priority");

        prFormCreateLink = Link("CREATE", "pr-form-create");

        prFormCancelLink = Link("CANCEL", "pr-form-cancel");
    }

    void init()
    {
        initStyles();
        initTabs();
        initBranches();
        initEntries();
        initIssues();
        initCommitForm();
        initPullRequests();
        initIssueForm();
        initPullRequestForm();
    }

    string queryPriorityText(Issue::Priority priority) const
    {
        switch (priority)
        {
        case Issue::Priority::High:
            return "HIGH";

        case Issue::Priority::Medium:
            return "MEDIUM";

        case Issue::Priority::Low:
            return "LOW";
        }

        return "";
    }

    string queryStateText(Issue::State state) const
    {
        switch (state)
        {
        case Issue::State::Open:
            return "OPEN";

        case Issue::State::Closed:
            return "CLOSED";
        }

        return "";
    }

    string issuePriorityText(Issue::Priority priority) const
    {
        switch (priority)
        {
        case Issue::Priority::High:
            return "HIGH";

        case Issue::Priority::Medium:
            return "MEDIUM";

        case Issue::Priority::Low:
            return "LOW";
        }

        return "";
    }

    string issueStateText(Issue::State state) const
    {
        switch (state)
        {
        case Issue::State::Open:
            return "OPEN";

        case Issue::State::Closed:
            return "CLOSED";
        }

        return "";
    }

    string pullRequestPriorityText(PullRequest::Priority priority) const
    {
        switch (priority)
        {
        case PullRequest::Priority::High:
            return "HIGH";

        case PullRequest::Priority::Medium:
            return "MEDIUM";

        case PullRequest::Priority::Low:
            return "LOW";
        }

        return "";
    }

    string pullRequestStateText(PullRequest::State state) const
    {
        switch (state)
        {
        case PullRequest::State::Open:
            return "OPEN";

        case PullRequest::State::Closed:
            return "CLOSED";
        }

        return "";
    }

    string truncate(const string &value, unsigned int width) const
    {
        if (value.length() <= width)
        {
            return value;
        }

        if (width <= 3)
        {
            return value.substr(0, width);
        }

        return value.substr(0, width - 3) + "...";
    }

    void updateIssueLinks()
    {
        string priorityLabel = "PRIORITY: ALL";

        if (issueQuery.filterPriority)
        {
            priorityLabel = "PRIORITY: " + queryPriorityText(issueQuery.priority);
        }

        string stateLabel = "STATE: ALL";

        if (issueQuery.filterState)
        {
            stateLabel = "STATE: " + queryStateText(issueQuery.state);
        }

        string sortLabel = "SORT: HIGH -> LOW";

        if (issueQuery.sortByPriority && issueQuery.ascending)
        {
            sortLabel = "SORT: LOW -> HIGH";
        }

        priorityFilterLink = Link(priorityLabel, "priority-filter");

        stateFilterLink = Link(stateLabel, "state-filter");

        sortPriorityLink = Link(sortLabel, "sort-priority");
    }

    void updatePullRequestLinks()
    {
        if (pullRequestQuery.filterPriority)
        {
            prPriorityFilterLink = Link("PRIORITY: " + pullRequestPriorityText(pullRequestQuery.priority), "pr-priority-filter");
        }
        else
        {
            prPriorityFilterLink = Link("PRIORITY: ALL", "pr-priority-filter");
        }

        if (pullRequestQuery.filterState)
        {
            prStateFilterLink = Link("STATE: " + pullRequestStateText(pullRequestQuery.state), "pr-state-filter");
        }
        else
        {
            prStateFilterLink = Link("STATE: ALL", "pr-state-filter");
        }

        if (pullRequestQuery.sortByPriority)
        {
            if (pullRequestQuery.ascending)
            {
                prSortPriorityLink = Link("SORT: LOW -> HIGH", "pr-sort-priority");
            }
            else
            {
                prSortPriorityLink = Link("SORT: HIGH -> LOW", "pr-sort-priority");
            }
        }
        else
        {
            prSortPriorityLink = Link("SORT: DEFAULT", "pr-sort-priority");
        }
    }

    void updateFormPriorityLink()
    {
        formPriorityLink = Link("PRIORITY: " + issuePriorityText(newIssuePriority), "form-priority");
    }

    void updatePullRequestFormLinks()
    {
        const vector<Branch> branches = repositoryVM.getBranches();

        if (branches.empty())
        {
            prSourceBranchLink = Link("SOURCE: NONE", "pr-source-branch");

            prTargetBranchLink = Link("TARGET: NONE", "pr-target-branch");
        }
        else
        {
            if (newPullRequestSourceBranch >= branches.size())
            {
                newPullRequestSourceBranch = 0;
            }

            if (newPullRequestTargetBranch >= branches.size())
            {
                newPullRequestTargetBranch = 0;
            }

            prSourceBranchLink = Link("SOURCE: " + branches[newPullRequestSourceBranch].name, "pr-source-branch");

            prTargetBranchLink = Link("TARGET: " + branches[newPullRequestTargetBranch].name, "pr-target-branch");
        }

        prFormPriorityLink = Link("PRIORITY: " + pullRequestPriorityText(newPullRequestPriority), "pr-form-priority");
    }

    void updateTabStyles()
    {
        for (size_t i = 0; i < tabButtons.size(); ++i)
        {
            const bool active = static_cast<size_t>(currentTab) == i;

            tabButtons[i].setStyles(active ? tabActiveStyle : tabNormalStyle, active ? tabActiveStyle : tabHoverStyle);
        }
    }

    void updateBranchStyles()
    {
        const vector<Branch> branches = repositoryVM.getBranches();

        for (size_t i = 0; i < branchButtons.size(); ++i)
        {
            const bool active = false;
            // branches[i].current;

            branchButtons[i].setStyles(active ? branchActiveStyle : branchNormalStyle, active ? branchActiveStyle : branchHoverStyle);
        }
    }

    void positionTabs(const Size &size)
    {
        const unsigned int spacing = 5;

        unsigned int totalWidth = 0;

        for (const Button &button : tabButtons)
        {
            totalWidth += button.getSize().width;
        }

        if (tabButtons.size() > 1)
        {
            totalWidth += spacing * (tabButtons.size() - 1);
        }

        unsigned int x = 1;

        if (size.width > totalWidth + 2)
        {
            x = (size.width - totalWidth) / 2;
        }

        for (Button &button : tabButtons)
        {
            button.updatePosition(Point(x, 1));

            x += button.getSize().width + spacing;
        }
    }

    void positionBranches()
    {
        unsigned int y = 8;

        for (Button &button : branchButtons)
        {
            button.updatePosition(Point(2, y));

            y += button.getSize().height + 1;
        }
    }

    void renderFrame(Canvas &canvas)
    {
        const Size size = canvas.getSize();

        if (size.width < 2 || size.height < 2)
        {
            return;
        }

        canvas.drawUTF8("╭", Point(0, 0), frameStyle);

        canvas.drawUTF8(_string(size.width - 2, "─"), Point(1, 0), frameStyle);

        canvas.drawUTF8("╮", Point(size.width - 1, 0), frameStyle);

        canvas.drawUTF8("╰", Point(0, size.height - 1), frameStyle);

        canvas.drawUTF8(_string(size.width - 2, "─"), Point(1, size.height - 1), frameStyle);

        canvas.drawUTF8("╯", Point(size.width - 1, size.height - 1), frameStyle);

        for (unsigned int y = 1; y < size.height - 1; ++y)
        {
            canvas.drawUTF8("│", Point(0, y), frameStyle);

            canvas.drawUTF8("│", Point(size.width - 1, y), frameStyle);
        }
    }

    void renderCodeActions(Canvas &canvas, const Size &size)
    {
        const unsigned int spacing = 2;

        const unsigned int folderWidth = static_cast<unsigned int>(splitUtf8("+ FOLDER").size());

        const unsigned int fileWidth = static_cast<unsigned int>(splitUtf8("+ FILE").size());

        const unsigned int totalWidth = folderWidth + fileWidth + spacing;

        unsigned int x = 2;

        if (size.width > totalWidth + 2)
        {
            x = size.width - totalWidth - 2;
        }

        createFolderLink.render(canvas, Point(x, 6));

        x += folderWidth + spacing;

        createFileLink.render(canvas, Point(x, 6));
    }

    void renderCode(Canvas &canvas)
    {
        const Size size = canvas.getSize();

        const unsigned int sidebarX = size.width / 4;

        const unsigned int contentX = sidebarX + 3;

        canvas.drawUTF8("BRANCHES", Point(2, 6), sectionStyle);

        positionBranches();

        for (Button &button : branchButtons)
        {
            button.render(canvas);
        }

        unsigned int branchY = 8;

        for (const Button &button : branchButtons)
        {
            branchY += button.getSize().height + 1;
        }

        createBranchLink.render(canvas, Point(2, branchY));

        createCommitLink.render(canvas, Point(2, branchY + 2));

        renderCodeActions(canvas, size);

        for (unsigned int y = 5; y < size.height - 1; ++y)
        {
            canvas.drawUTF8("│", Point(sidebarX, y), frameStyle);
        }

        unsigned int y = 6;

        for (Link &entryLink : entryLinks)
        {
            entryLink.render(canvas, Point(contentX, y));

            y += 3;
        }
    }

    void renderCommits(Canvas &canvas)
    {
        const Size size = canvas.getSize();

        const vector<Commit> commits = repositoryVM.getLog();

        canvas.drawUTF8("COMMITS", Point(2, 6), sectionStyle);

        const unsigned int idX = 2;
        const unsigned int titleX = 14;
        const unsigned int descriptionX = 38;
        const unsigned int parentX = size.width > 75 ? size.width - 18 : 60;

        canvas.drawUTF8("ID", Point(idX, 9), sectionStyle);

        canvas.drawUTF8("TITLE", Point(titleX, 9), sectionStyle);

        canvas.drawUTF8("DESCRIPTION", Point(descriptionX, 9), sectionStyle);

        canvas.drawUTF8("PARENT", Point(parentX, 9), sectionStyle);

        canvas.drawUTF8(_string(size.width - 4, "─"), Point(2, 10), frameStyle);

        unsigned int y = 12;

        for (const Commit &commit : commits)
        {
            if (y >= size.height - 2)
            {
                break;
            }

            canvas.drawUTF8(truncate(commit.id, 10), Point(idX, y), entryNormalStyle);

            canvas.drawUTF8(truncate(commit.title, 20), Point(titleX, y), entryNormalStyle);

            canvas.drawUTF8(truncate(commit.description, 30), Point(descriptionX, y), entryNormalStyle);

            canvas.drawUTF8(truncate(commit.parentId.empty() ? "-" : commit.parentId, 12), Point(parentX, y), entryNormalStyle);

            y += 2;
        }
    }

    void renderIssueHeader(Canvas &canvas)
    {
        const Size size = canvas.getSize();

        canvas.drawUTF8("ISSUES", Point(2, 6), sectionStyle);

        priorityFilterLink.render(canvas, Point(2, 8));

        stateFilterLink.render(canvas, Point(20, 8));

        sortPriorityLink.render(canvas, Point(36, 8));

        createIssueLink.render(canvas, Point(size.width - 16, 8));
    }

    void renderIssueTable(Canvas &canvas)
    {
        const Size size = canvas.getSize();

        const vector<Issue> issues = repositoryVM.getIssues(issueQuery);

        const unsigned int idX = 2;
        const unsigned int titleX = 14;
        const unsigned int descriptionX = 36;

        const unsigned int priorityX = size.width > 70 ? size.width - 25 : 45;

        const unsigned int stateX = size.width > 70 ? size.width - 12 : 56;

        canvas.drawUTF8("ID", Point(idX, 11), sectionStyle);

        canvas.drawUTF8("TITLE", Point(titleX, 11), sectionStyle);

        canvas.drawUTF8("DESCRIPTION", Point(descriptionX, 11), sectionStyle);

        canvas.drawUTF8("PRIORITY", Point(priorityX, 11), sectionStyle);

        canvas.drawUTF8("STATE", Point(stateX, 11), sectionStyle);

        canvas.drawUTF8(_string(size.width - 4, "─"), Point(2, 12), frameStyle);

        unsigned int y = 14;

        for (const Issue &issue : issues)
        {
            if (y >= size.height - 2)
            {
                break;
            }

            canvas.drawUTF8(truncate(issue.id, 10), Point(idX, y), entryNormalStyle);

            canvas.drawUTF8(truncate(issue.title, 20), Point(titleX, y), entryNormalStyle);

            canvas.drawUTF8(truncate(issue.description, 28), Point(descriptionX, y), entryNormalStyle);

            canvas.drawUTF8(issuePriorityText(issue.priority), Point(priorityX, y), entryNormalStyle);

            canvas.drawUTF8(issueStateText(issue.state), Point(stateX, y), entryNormalStyle);

            y += 2;
        }
    }

    void renderIssues(Canvas &canvas)
    {
        renderIssueHeader(canvas);
        renderIssueTable(canvas);
    }

    void renderPullRequestHeader(Canvas &canvas)
    {
        const Size size = canvas.getSize();

        canvas.drawUTF8("PULL REQUESTS", Point(2, 6), sectionStyle);

        prPriorityFilterLink.render(canvas, Point(2, 8));

        prStateFilterLink.render(canvas, Point(20, 8));

        prSortPriorityLink.render(canvas, Point(36, 8));

        createPullRequestLink.render(canvas, Point(size.width - 25, 8));
    }

    void renderPullRequestTable(Canvas &canvas)
    {
        const Size size = canvas.getSize();

        const vector<PullRequest> pullRequests = repositoryVM.getPullRequests(pullRequestQuery);

        const unsigned int idX = 2;
        const unsigned int titleX = 12;
        const unsigned int descriptionX = 32;
        const unsigned int sourceX = 58;
        const unsigned int targetX = 72;
        const unsigned int priorityX = 84;
        const unsigned int stateX = 96;

        canvas.drawUTF8("ID", Point(idX, 11), sectionStyle);

        canvas.drawUTF8("TITLE", Point(titleX, 11), sectionStyle);

        canvas.drawUTF8("DESCRIPTION", Point(descriptionX, 11), sectionStyle);

        canvas.drawUTF8("SOURCE", Point(sourceX, 11), sectionStyle);

        canvas.drawUTF8("TARGET", Point(targetX, 11), sectionStyle);

        canvas.drawUTF8("PRIORITY", Point(priorityX, 11), sectionStyle);

        canvas.drawUTF8("STATE", Point(stateX, 11), sectionStyle);

        canvas.drawUTF8(_string(size.width - 4, "─"), Point(2, 12), frameStyle);

        unsigned int y = 14;

        for (const PullRequest &pullRequest : pullRequests)
        {
            if (y >= size.height - 2)
            {
                break;
            }

            canvas.drawUTF8(truncate(pullRequest.id, 8), Point(idX, y), entryNormalStyle);

            canvas.drawUTF8(truncate(pullRequest.title, 18), Point(titleX, y), entryNormalStyle);

            canvas.drawUTF8(truncate(pullRequest.description, 24), Point(descriptionX, y), entryNormalStyle);

            canvas.drawUTF8(truncate(pullRequest.sourceBranch.name, 12), Point(sourceX, y), entryNormalStyle);

            canvas.drawUTF8(truncate(pullRequest.targetBranch.name, 12), Point(targetX, y), entryNormalStyle);

            canvas.drawUTF8(pullRequestPriorityText(pullRequest.priority), Point(priorityX, y), entryNormalStyle);

            canvas.drawUTF8(pullRequestStateText(pullRequest.state), Point(stateX, y), entryNormalStyle);

            y += 2;
        }
    }

    void renderPullRequests(Canvas &canvas)
    {
        renderPullRequestHeader(canvas);
        renderPullRequestTable(canvas);
    }

    void renderIssueForm(Canvas &canvas)
    {
        const Size size = canvas.getSize();

        const unsigned int formWidth = 58;

        const unsigned int formX = size.width > formWidth ? (size.width - formWidth) / 2 : 2;

        const unsigned int formY = 7;

        canvas.drawUTF8("CREATE ISSUE", Point(formX, formY), sectionStyle);

        canvas.drawUTF8("TITLE", Point(formX, formY + 2), sectionStyle);

        issueTitleField.render(canvas, Point(formX, formY + 3));

        canvas.drawUTF8("DESCRIPTION", Point(formX, formY + 7), sectionStyle);

        issueDescriptionField.render(canvas, Point(formX, formY + 8));

        canvas.drawUTF8("PRIORITY", Point(formX, formY + 12), sectionStyle);

        formPriorityLink.render(canvas, Point(formX, formY + 13));

        formCreateLink.render(canvas, Point(formX + 38, formY + 13));

        formCancelLink.render(canvas, Point(formX + 47, formY + 13));
    }

    void renderCommitForm(Canvas &canvas)
    {
        const Size size = canvas.getSize();

        const unsigned int formWidth = 58;

        const unsigned int formX = size.width > formWidth ? (size.width - formWidth) / 2 : 2;

        const unsigned int formY = 7;

        canvas.drawUTF8("CREATE COMMIT", Point(formX, formY), sectionStyle);

        canvas.drawUTF8("TITLE", Point(formX, formY + 2), sectionStyle);

        commitTitleField.render(canvas, Point(formX, formY + 3));

        canvas.drawUTF8("DESCRIPTION", Point(formX, formY + 7), sectionStyle);

        commitDescriptionField.render(canvas, Point(formX, formY + 8));

        formCreateLink.render(canvas, Point(formX + 38, formY + 13));

        formCancelLink.render(canvas, Point(formX + 47, formY + 13));
    }

    void renderPullRequestForm(Canvas &canvas)
    {
        const Size size = canvas.getSize();

        const unsigned int formWidth = 58;

        const unsigned int formX = size.width > formWidth ? (size.width - formWidth) / 2 : 2;

        const unsigned int formY = 7;

        canvas.drawUTF8("CREATE PULL REQUEST", Point(formX, formY), sectionStyle);

        canvas.drawUTF8("TITLE", Point(formX, formY + 2), sectionStyle);

        pullRequestTitleField.render(canvas, Point(formX, formY + 3));

        canvas.drawUTF8("DESCRIPTION", Point(formX, formY + 7), sectionStyle);

        pullRequestDescriptionField.render(canvas, Point(formX, formY + 8));

        canvas.drawUTF8("SOURCE BRANCH", Point(formX, formY + 12), sectionStyle);

        prSourceBranchLink.render(canvas, Point(formX, formY + 13));

        canvas.drawUTF8("TARGET BRANCH", Point(formX, formY + 16), sectionStyle);

        prTargetBranchLink.render(canvas, Point(formX, formY + 17));

        canvas.drawUTF8("PRIORITY", Point(formX, formY + 20), sectionStyle);

        prFormPriorityLink.render(canvas, Point(formX, formY + 21));

        prFormCreateLink.render(canvas, Point(formX + 38, formY + 21));

        prFormCancelLink.render(canvas, Point(formX + 47, formY + 21));
    }

    void renderCreationForm(Canvas &canvas)
    {
        const Size size = canvas.getSize();

        const unsigned int formWidth = 44;

        const unsigned int formX = size.width > formWidth ? (size.width - formWidth) / 2 : 2;

        const unsigned int formY = 8;

        string title;

        switch (currentForm)
        {
        case Form::Branch:
            title = "CREATE BRANCH";
            break;

        case Form::Directory:
            title = "CREATE FOLDER";
            break;

        case Form::File:
            title = "CREATE FILE";
            break;

        default:
            return;
        }

        canvas.drawUTF8(title, Point(formX, formY), sectionStyle);

        creationNameField.render(canvas, Point(formX, formY + 3));

        formCreateLink.render(canvas, Point(formX, formY + 8));

        formCancelLink.render(canvas, Point(formX + 10, formY + 8));
    }

    void renderCurrentTab(Canvas &canvas)
    {
        if (currentForm == Form::Issue)
        {
            renderIssueForm(canvas);
            return;
        }

        if (currentForm == Form::Commit)
        {
            renderCommitForm(canvas);
            return;
        }

        if (currentForm == Form::PullRequest)
        {
            renderPullRequestForm(canvas);
            return;
        }

        if (currentForm == Form::Branch || currentForm == Form::Directory || currentForm == Form::File)
        {
            renderCreationForm(canvas);
            return;
        }

        switch (currentTab)
        {
        case Tab::Code:
            renderCode(canvas);
            break;

        case Tab::Commits:
            renderCommits(canvas);
            break;

        case Tab::Issues:
            renderIssues(canvas);
            break;

        case Tab::PullRequest:
            renderPullRequests(canvas);
            break;
        }
    }

    void openCreationForm(Form form)
    {
        currentForm = form;

        string placeholder;

        switch (form)
        {
        case Form::Branch:
            placeholder = "Branch name";
            break;

        case Form::Directory:
            placeholder = "Folder name";
            break;

        case Form::File:
            placeholder = "File name";
            break;

        default:
            return;
        }

        creationNameField = InputField("name", placeholder, 100, 40);

        formCreateLink = Link("CREATE", "form-create");

        formCancelLink = Link("CANCEL", "form-cancel");

        creationNameField.focus();
    }

    void closeCreationForm()
    {
        currentForm = Form::None;

        creationNameField.setValue("");
        creationNameField.blur();
    }

    void createFromCreationForm()
    {
        const string name = creationNameField.getValue();

        if (name.empty())
        {
            return;
        }

        switch (currentForm)
        {
        case Form::Branch:
            repositoryVM.createBranch(name);
            initBranches();
            updateBranchStyles();
            break;

        case Form::Directory:
            repositoryVM.createDirectory(name);
            initEntries();
            break;

        case Form::File:
            repositoryVM.createFile(name);
            initEntries();
            break;

        default:
            return;
        }

        closeCreationForm();
    }

    void openIssueForm()
    {
        currentForm = Form::Issue;

        issueTitleField.setValue("");
        issueDescriptionField.setValue("");

        issueTitleField.blur();
        issueDescriptionField.blur();

        newIssuePriority = Issue::Priority::High;

        updateFormPriorityLink();

        formCreateLink = Link("CREATE", "form-create");

        formCancelLink = Link("CANCEL", "form-cancel");
    }

    void closeIssueForm()
    {
        currentForm = Form::None;

        issueTitleField.setValue("");
        issueDescriptionField.setValue("");

        issueTitleField.blur();
        issueDescriptionField.blur();
    }

    void openCommitForm()
    {
        currentForm = Form::Commit;

        commitTitleField.setValue("");
        commitDescriptionField.setValue("");

        commitTitleField.blur();
        commitDescriptionField.blur();

        formCreateLink = Link("CREATE", "form-create");

        formCancelLink = Link("CANCEL", "form-cancel");
    }

    void closeCommitForm()
    {
        currentForm = Form::None;

        commitTitleField.setValue("");
        commitDescriptionField.setValue("");

        commitTitleField.blur();
        commitDescriptionField.blur();
    }

    void createCommitFromForm()
    {
        const string title = commitTitleField.getValue();

        const string description = commitDescriptionField.getValue();

        if (title.empty())
        {
            return;
        }

        repositoryVM.createCommit(title, description);

        closeCommitForm();
    }

    void cycleIssuePriority()
    {
        switch (newIssuePriority)
        {
        case Issue::Priority::High:
            newIssuePriority = Issue::Priority::Medium;
            break;

        case Issue::Priority::Medium:
            newIssuePriority = Issue::Priority::Low;
            break;

        case Issue::Priority::Low:
            newIssuePriority = Issue::Priority::High;
            break;
        }

        updateFormPriorityLink();
    }

    void openPullRequestForm()
    {
        currentForm = Form::PullRequest;

        pullRequestTitleField.setValue("");
        pullRequestDescriptionField.setValue("");

        pullRequestTitleField.blur();
        pullRequestDescriptionField.blur();

        newPullRequestPriority = PullRequest::Priority::High;

        newPullRequestSourceBranch = 0;
        newPullRequestTargetBranch = 0;

        const vector<Branch> branches = repositoryVM.getBranches();

        if (branches.size() > 1)
        {
            newPullRequestTargetBranch = 1;
        }

        updatePullRequestFormLinks();
    }

    void closePullRequestForm()
    {
        currentForm = Form::None;

        pullRequestTitleField.setValue("");
        pullRequestDescriptionField.setValue("");

        pullRequestTitleField.blur();
        pullRequestDescriptionField.blur();
    }

    void cyclePullRequestPriority()
    {
        switch (newPullRequestPriority)
        {
        case PullRequest::Priority::High:
            newPullRequestPriority = PullRequest::Priority::Medium;
            break;

        case PullRequest::Priority::Medium:
            newPullRequestPriority = PullRequest::Priority::Low;
            break;

        case PullRequest::Priority::Low:
            newPullRequestPriority = PullRequest::Priority::High;
            break;
        }

        updatePullRequestFormLinks();
    }

    void cycleSourceBranch()
    {
        const vector<Branch> branches = repositoryVM.getBranches();

        if (branches.empty())
        {
            return;
        }

        newPullRequestSourceBranch++;

        if (newPullRequestSourceBranch >= branches.size())
        {
            newPullRequestSourceBranch = 0;
        }

        if (newPullRequestSourceBranch == newPullRequestTargetBranch && branches.size() > 1)
        {
            newPullRequestSourceBranch++;

            if (newPullRequestSourceBranch >= branches.size())
            {
                newPullRequestSourceBranch = 0;
            }
        }

        updatePullRequestFormLinks();
    }

    void cycleTargetBranch()
    {
        const vector<Branch> branches = repositoryVM.getBranches();

        if (branches.empty())
        {
            return;
        }

        newPullRequestTargetBranch++;

        if (newPullRequestTargetBranch >= branches.size())
        {
            newPullRequestTargetBranch = 0;
        }

        if (newPullRequestTargetBranch == newPullRequestSourceBranch && branches.size() > 1)
        {
            newPullRequestTargetBranch++;

            if (newPullRequestTargetBranch >= branches.size())
            {
                newPullRequestTargetBranch = 0;
            }
        }

        updatePullRequestFormLinks();
    }

    void handleTabs(const Event &event)
    {
        for (size_t i = 0; i < tabButtons.size(); ++i)
        {
            tabButtons[i].onClick(event, [this, i]()
            {
                currentTab = static_cast<Tab>(i);

                currentForm = Form::None;

                updateTabStyles();
            });
        }
    }

    void handleBranches(const Event &event)
    {
        const vector<Branch> branches = repositoryVM.getBranches();

        for (size_t i = 0; i < branchButtons.size(); ++i)
        {
            branchButtons[i].onClick(event, [this, branches, i]()
            {
                repositoryVM.switchBranch(branches[i].name);

                initEntries();

                updateBranchStyles();
            });
        }

        createBranchLink.handle(event, [this](const string &)
        {
            openCreationForm(Form::Branch);
        });

        createCommitLink.handle(event, [this](const string &)
        {
            openCommitForm();
        });
    }

    void handleCodeActions(const Event &event)
    {
        createFolderLink.handle(event, [this](const string &)
        {
            openCreationForm(Form::Directory);
        });

        createFileLink.handle(event, [this](const string &)
        {
            openCreationForm(Form::File);
        });
    }

    void handleEntries(const Event &event)
    {
        for (Link &entryLink : entryLinks)
        {
            entryLink.handle(event, [this](const string &entryName)
            {
                repositoryVM.open(entryName);

                initEntries();
            });
        }
    }

    void handleCreationForm(const Event &event)
    {
        if (event.type == EventType::MouseDown)
        {
            Point mousePosition(static_cast<unsigned int>(event.mouseX), static_cast<unsigned int>(event.mouseY));

            if (creationNameField.contains(mousePosition))
            {
                creationNameField.focus();
            }
            else
            {
                creationNameField.blur();
            }
        }

        creationNameField.handle(event);

        formCancelLink.handle(event, [this](const string &)
        {
            closeCreationForm();
        });

        formCreateLink.handle(event, [this](const string &)
        {
            createFromCreationForm();
        });
    }

    void handleIssueFilters(const Event &event)
    {
        priorityFilterLink.handle(event, [this](const string &)
        {
            if (!issueQuery.filterPriority)
            {
                issueQuery.filterPriority = true;
                issueQuery.priority = Issue::Priority::High;
            }
            else
            {
                switch (issueQuery.priority)
                {
                case Issue::Priority::High:
                    issueQuery.priority = Issue::Priority::Medium;
                    break;

                case Issue::Priority::Medium:
                    issueQuery.priority = Issue::Priority::Low;
                    break;

                case Issue::Priority::Low:
                    issueQuery.filterPriority = false;
                    break;
                }
            }

            updateIssueLinks();
        });

        stateFilterLink.handle(event, [this](const string &)
        {
            if (!issueQuery.filterState)
            {
                issueQuery.filterState = true;
                issueQuery.state = Issue::State::Open;
            }
            else
            {
                switch (issueQuery.state)
                {
                case Issue::State::Open:
                    issueQuery.state = Issue::State::Closed;
                    break;

                case Issue::State::Closed:
                    issueQuery.filterState = false;
                    break;
                }
            }

            updateIssueLinks();
        });

        sortPriorityLink.handle(event, [this](const string &)
        {
            if (!issueQuery.sortByPriority)
            {
                issueQuery.sortByPriority = true;
                issueQuery.ascending = false;
            }
            else
            {
                issueQuery.ascending = !issueQuery.ascending;
            }

            updateIssueLinks();
        });

        createIssueLink.handle(event, [this](const string &)
        {
            openIssueForm();
        });
    }

    void handlePullRequestFilters(const Event &event)
    {
        prPriorityFilterLink.handle(event, [this](const string &)
        {
            if (!pullRequestQuery.filterPriority)
            {
                pullRequestQuery.filterPriority = true;

                pullRequestQuery.priority = PullRequest::Priority::High;
            }
            else
            {
                switch (pullRequestQuery.priority)
                {
                case PullRequest::Priority::High:
                    pullRequestQuery.priority = PullRequest::Priority::Medium;
                    break;

                case PullRequest::Priority::Medium:
                    pullRequestQuery.priority = PullRequest::Priority::Low;
                    break;

                case PullRequest::Priority::Low:
                    pullRequestQuery.filterPriority = false;

                    pullRequestQuery.priority = PullRequest::Priority::High;
                    break;
                }
            }

            updatePullRequestLinks();
        });

        prStateFilterLink.handle(event, [this](const string &)
        {
            if (!pullRequestQuery.filterState)
            {
                pullRequestQuery.filterState = true;

                pullRequestQuery.state = PullRequest::State::Open;
            }
            else if (pullRequestQuery.state == PullRequest::State::Open)
            {
                pullRequestQuery.state = PullRequest::State::Closed;
            }
            else
            {
                pullRequestQuery.filterState = false;

                pullRequestQuery.state = PullRequest::State::Open;
            }

            updatePullRequestLinks();
        });

        prSortPriorityLink.handle(event, [this](const string &)
        {
            if (!pullRequestQuery.sortByPriority)
            {
                pullRequestQuery.sortByPriority = true;

                pullRequestQuery.ascending = false;
            }
            else if (!pullRequestQuery.ascending)
            {
                pullRequestQuery.ascending = true;
            }
            else
            {
                pullRequestQuery.sortByPriority = false;

                pullRequestQuery.ascending = false;
            }

            updatePullRequestLinks();
        });

        createPullRequestLink.handle(event, [this](const string &)
        {
            openPullRequestForm();
        });
    }

    void handleIssueForm(const Event &event)
    {
        if (event.type == EventType::MouseDown)
        {
            Point mousePosition(static_cast<unsigned int>(event.mouseX), static_cast<unsigned int>(event.mouseY));

            if (issueTitleField.contains(mousePosition))
            {
                issueTitleField.focus();
                issueDescriptionField.blur();
            }
            else if (issueDescriptionField.contains(mousePosition))
            {
                issueDescriptionField.focus();
                issueTitleField.blur();
            }
        }

        issueTitleField.handle(event);

        issueDescriptionField.handle(event);

        formPriorityLink.handle(event, [this](const string &)
        {
            cycleIssuePriority();
        });

        formCancelLink.handle(event, [this](const string &)
        {
            closeIssueForm();
        });

        formCreateLink.handle(event, [this](const string &)
        {
            const string title = issueTitleField.getValue();

            const string description = issueDescriptionField.getValue();

            repositoryVM.createIssue(title, description, newIssuePriority);

            closeIssueForm();
        });
    }

    void handleCommitForm(const Event &event)
    {
        if (event.type == EventType::MouseDown)
        {
            Point mousePosition(static_cast<unsigned int>(event.mouseX), static_cast<unsigned int>(event.mouseY));

            if (commitTitleField.contains(mousePosition))
            {
                commitTitleField.focus();
                commitDescriptionField.blur();
            }
            else if (commitDescriptionField.contains(mousePosition))
            {
                commitDescriptionField.focus();
                commitTitleField.blur();
            }
        }

        commitTitleField.handle(event);

        commitDescriptionField.handle(event);

        formCancelLink.handle(event, [this](const string &)
        {
            closeCommitForm();
        });

        formCreateLink.handle(event, [this](const string &)
        {
            createCommitFromForm();
        });
    }

    void handlePullRequestForm(const Event &event)
    {
        if (event.type == EventType::MouseDown)
        {
            Point mousePosition(static_cast<unsigned int>(event.mouseX), static_cast<unsigned int>(event.mouseY));

            if (pullRequestTitleField.contains(mousePosition))
            {
                pullRequestTitleField.focus();
                pullRequestDescriptionField.blur();
            }
            else if (pullRequestDescriptionField.contains(mousePosition))
            {
                pullRequestDescriptionField.focus();
                pullRequestTitleField.blur();
            }
        }

        pullRequestTitleField.handle(event);

        pullRequestDescriptionField.handle(event);

        prSourceBranchLink.handle(event, [this](const string &)
        {
            cycleSourceBranch();
        });

        prTargetBranchLink.handle(event, [this](const string &)
        {
            cycleTargetBranch();
        });

        prFormPriorityLink.handle(event, [this](const string &)
        {
            cyclePullRequestPriority();
        });

        prFormCancelLink.handle(event, [this](const string &)
        {
            closePullRequestForm();
        });

        prFormCreateLink.handle(event, [this](const string &)
        {
            const string title = pullRequestTitleField.getValue();

            const string description = pullRequestDescriptionField.getValue();

            const vector<Branch> branches = repositoryVM.getBranches();

            if (branches.size() < 2)
            {
                return;
            }

            const Branch sourceBranch = branches[newPullRequestSourceBranch];

            const Branch targetBranch = branches[newPullRequestTargetBranch];

            repositoryVM.createPullRequest(title, description, sourceBranch.name, targetBranch.name, newPullRequestPriority);

            closePullRequestForm();
        });
    }

public:
    RepositoryView(const string &repositoryId) : repositoryVM(repositoryId)
    {
        init();
    }

    void render(Canvas &canvas)
    {
        const Size size = canvas.getSize();

        updateTabStyles();

        updateBranchStyles();

        renderFrame(canvas);

        positionTabs(size);

        for (Button &button : tabButtons)
        {
            button.render(canvas);
        }

        canvas.drawUTF8(_string(size.width - 2, "─"), Point(1, 4), frameStyle);

        renderCurrentTab(canvas);
    }

    void handle(const Event &event, NavigationStack &ns)
    {
        handleTabs(event);

        if (currentForm == Form::Branch || currentForm == Form::Directory || currentForm == Form::File)
        {
            handleCreationForm(event);
            return;
        }

        if (currentForm == Form::Issue)
        {
            handleIssueForm(event);
            return;
        }

        if (currentForm == Form::Commit)
        {
            handleCommitForm(event);
            return;
        }

        if (currentForm == Form::PullRequest)
        {
            handlePullRequestForm(event);
            return;
        }

        if (currentTab == Tab::Code)
        {
            handleBranches(event);

            handleCodeActions(event);

            handleEntries(event);
        }
        else if (currentTab == Tab::Issues)
        {
            handleIssueFilters(event);
        }
        else if (currentTab == Tab::PullRequest)
        {
            handlePullRequestFilters(event);
        }
    }
};